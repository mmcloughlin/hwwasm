macro_rules! declare_index {
    ($name:ident) => {
        #[derive(Copy, Clone, Debug)]
        pub struct $name(pub usize);

        impl $name {
            pub fn index(&self) -> usize {
                self.0
            }
        }
    };
}

declare_index!(LocalIdx);

#[derive(Debug)]
pub enum Type {
    Int(usize),
}

#[derive(Debug)]
pub struct Bits {
    pub width: usize,
    pub value: u128,
}

impl Bits {
    pub fn new(width: usize, value: u128) -> Bits {
        Bits { width, value }
    }
}

#[derive(Debug)]
pub enum Inst {
    // Variables
    LocalGet { idx: LocalIdx },
    LocalSet { idx: LocalIdx },

    // Numerics
    IConst { bits: Bits },
    IAdd,
    IAnd,
    IXor,
    IRotl,

    // Pseudo
    Extract { low: usize, width: usize },
}

#[derive(Debug)]
pub struct Function {
    pub locals: Vec<Type>,
    pub insts: Vec<Inst>,
}

impl Function {
    pub fn new() -> Function {
        Function {
            locals: Vec::new(),
            insts: Vec::new(),
        }
    }

    pub fn alloc_local(&mut self, ty: Type) -> LocalIdx {
        let idx = LocalIdx(self.locals.len());
        self.locals.push(ty);
        idx
    }
}
