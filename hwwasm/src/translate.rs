use std::collections::HashMap;

use crate::ir::{self, Inst};
use anyhow::{bail, format_err, Result};
use hwwasm_aslp::ast::{Block, Expr, LExpr, Slice, Stmt, Type};

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub enum Target {
    Var(String),
    Index(Box<Target>, usize),
    Field(Box<Target>, String),
}

impl TryFrom<&LExpr> for Target {
    type Error = anyhow::Error;

    fn try_from(lexpr: &LExpr) -> Result<Self> {
        match lexpr {
            LExpr::Var(v) => Ok(Target::Var(v.clone())),
            LExpr::ArrayIndex { array, index } => {
                let array = Box::new(array.as_ref().try_into()?);
                let index = index
                    .as_lit_int()
                    .ok_or(format_err!("array index must be literal integer"))?
                    .parse()?;
                Ok(Target::Index(array, index))
            }
            LExpr::Field { x, name } => {
                let x = Box::new(x.as_ref().try_into()?);
                Ok(Target::Field(x, name.clone()))
            }
        }
    }
}

impl TryFrom<&Expr> for Target {
    type Error = anyhow::Error;

    fn try_from(expr: &Expr) -> Result<Self> {
        match expr {
            Expr::Var(v) => Ok(Target::Var(v.clone())),
            Expr::ArrayIndex { array, index } => {
                let array = Box::new(array.as_ref().try_into()?);
                let index = index
                    .as_lit_int()
                    .ok_or(format_err!("array index must be literal integer"))?
                    .parse()?;
                Ok(Target::Index(array, index))
            }
            Expr::Field { x, name } => {
                let x = Box::new(x.as_ref().try_into()?);
                Ok(Target::Field(x, name.clone()))
            }
            _ => todo!("target expr: {expr:?}"),
        }
    }
}

struct Scope {
    target_local: HashMap<Target, ir::LocalIdx>,
}

impl Scope {
    fn new() -> Scope {
        Scope {
            target_local: HashMap::new(),
        }
    }
}

pub struct Translator {
    func: ir::Function,
    scope: Scope,
}

impl Translator {
    pub fn new() -> Translator {
        Translator {
            func: ir::Function::new(),
            scope: Scope::new(),
        }
    }

    pub fn function(&self) -> &ir::Function {
        &self.func
    }

    pub fn arg(&mut self, ty: ir::Type, target: Target) {
        assert!(self.func.insts.is_empty());
        let idx = self.func.alloc_local(ty);
        self.scope.target_local.insert(target, idx);
    }

    pub fn translate(&mut self, block: &Block) -> Result<()> {
        for stmt in &block.stmts {
            self.stmt(stmt)?;
        }
        Ok(())
    }

    fn stmt(&mut self, stmt: &Stmt) -> Result<()> {
        match stmt {
            Stmt::ConstDecl { ty, name, rhs } => {
                // Allocate variable for the declared constant.
                let ty = ir::Type::try_from(ty)?;
                let idx = self.func.alloc_local(ty);
                self.scope
                    .target_local
                    .insert(Target::Var(name.clone()), idx);

                // Evaluate and assign.
                self.expr(rhs)?;
                self.emit(Inst::LocalSet { idx });
            }
            //Stmt::VarDecl { ty, name, rhs } => todo!(),
            //Stmt::VarDeclsNoInit { ty, names } => todo!(),
            Stmt::Assign { lhs, rhs } => {
                // Lookup allocated variable for the LHS.
                let target = lhs.try_into()?;
                let idx = *self
                    .scope
                    .target_local
                    .get(&target)
                    .ok_or_else(|| format_err!("assignment to undefined: {target:?}"))?;

                // Evaluate and assign.
                self.expr(rhs)?;
                self.emit(Inst::LocalSet { idx });
            }
            //Stmt::Assert { cond } => todo!(),
            //Stmt::If {
            //    cond,
            //    then_block,
            //    else_block,
            //} => todo!(),
            //Stmt::Call { func, types, args } => todo!(),
            s => todo!("statement: {s:?}"),
        }
        Ok(())
    }

    fn expr(&mut self, expr: &Expr) -> Result<()> {
        match expr {
            Expr::Apply { func, types, args } => self.apply(&func.name, types, args),
            Expr::Var(..) | Expr::ArrayIndex { .. } | Expr::Field { .. } => {
                let target: Target = expr.try_into()?;
                let idx = *self
                    .scope
                    .target_local
                    .get(&target)
                    .ok_or_else(|| format_err!("undefined read: {target:?}"))?;
                self.emit(Inst::LocalGet { idx });
                Ok(())
            }
            Expr::Slices { x, slices } => {
                let slice = expect_unary(slices)?;
                self.slice(x, slice)
            }
            //Expr::LitInt(_) => todo!(),
            //Expr::LitBits(_) => todo!(),
            e => todo!("expression: {e:?}"),
        }
    }

    fn apply(&mut self, func: &str, types: &[Expr], args: &[Expr]) -> Result<()> {
        match func {
            "add_bits" => self.binary(Inst::IAdd, args),
            "and_bits" => self.binary(Inst::IAnd, args),
            "eor_bits" => self.binary(Inst::IXor, args),
            "rol_bits" => {
                let (x, s) = expect_binary(args)?;

                // ASLp produces a shift amount bit width that may be less than
                // the width of the value to be shifted.
                let (xw, sw) = expect_binary_types(types)?;
                if sw > xw {
                    bail!("shift amount width greater than value width");
                }

                // Expect shift amount to be constant, and not exceeding operand width.
                let s = expr_lit_bits_as_u128(s)?;
                if s >= xw as u128 {
                    bail!("shift amount {s} greater than value width {xw}");
                }

                // Emit the shift operation.
                self.expr(x)?;
                self.emit(Inst::IConst {
                    bits: ir::Bits::new(xw, s),
                });
                self.emit(Inst::IRotl);
                Ok(())
            }
            _ => todo!("function: {func:?}"),
        }
    }

    fn binary(&mut self, inst: ir::Inst, args: &[Expr]) -> Result<()> {
        let (x, y) = expect_binary(args)?;
        self.expr(x)?;
        self.expr(y)?;
        self.emit(inst);
        Ok(())
    }

    fn slice(&mut self, x: &Expr, slice: &Slice) -> Result<()> {
        match slice {
            Slice::LowWidth(l, w) => {
                let low = expr_lit_int_as_usize(l)?;
                let width = expr_lit_int_as_usize(w)?;
                self.expr(x)?;
                self.emit(Inst::Extract { low, width });
            }
        }
        Ok(())
    }

    fn emit(&mut self, inst: Inst) {
        self.func.insts.push(inst);
    }
}

impl TryFrom<&Type> for ir::Type {
    type Error = anyhow::Error;

    fn try_from(ty: &Type) -> Result<Self, Self::Error> {
        match ty {
            Type::Bits(width) => Ok(ir::Type::Int(expr_lit_int_as_usize(width)?)),
            _ => todo!("type: {ty:?}"),
        }
    }
}

fn expect_unary<T>(xs: &[T]) -> Result<&T> {
    if xs.len() != 1 {
        bail!("expected unary");
    }
    Ok(&xs[0])
}

fn expect_binary<T>(xs: &[T]) -> Result<(&T, &T)> {
    if xs.len() != 2 {
        bail!("expected binary");
    }
    Ok((&xs[0], &xs[1]))
}

fn expect_binary_types(types: &[Expr]) -> Result<(usize, usize)> {
    let (t1, t2) = expect_binary(types)?;
    Ok((expr_lit_int_as_usize(t1)?, expr_lit_int_as_usize(t2)?))
}

fn expr_lit_bits_as_u128(expr: &Expr) -> Result<u128> {
    Ok(u128::from_str_radix(
        expr.as_lit_bits()
            .ok_or(format_err!("expected literal bits"))?,
        2,
    )?)
}

fn expr_lit_int_as_usize(expr: &Expr) -> Result<usize> {
    Ok(expr
        .as_lit_int()
        .ok_or(format_err!("expected literal integer"))?
        .parse()?)
}
