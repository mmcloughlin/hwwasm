use hwwasm_aslp::ast::{Block, Stmt};

struct Scope {}

pub struct Translator {
    stack: Vec<Scope>,
}

impl Translator {
    pub fn new() -> Translator {
        let global = Scope {};
        Translator {
            stack: vec![global],
        }
    }

    pub fn translate(&self, block: &Block) {
        for stmt in &block.stmts {
            self.stmt(stmt);
        }
    }

    fn stmt(&self, stmt: &Stmt) {
        match stmt {
            //Stmt::ConstDecl { ty, name, rhs } => todo!(),
            //Stmt::VarDecl { ty, name, rhs } => todo!(),
            //Stmt::VarDeclsNoInit { ty, names } => todo!(),
            //Stmt::Assign { lhs, rhs } => todo!(),
            //Stmt::Assert { cond } => todo!(),
            //Stmt::If {
            //    cond,
            //    then_block,
            //    else_block,
            //} => todo!(),
            //Stmt::Call { func, types, args } => todo!(),
            s => todo!("statement: {s:?}"),
        }
    }
}
