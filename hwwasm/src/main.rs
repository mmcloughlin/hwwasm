use anyhow::Result;
use clap::Parser as ClapParser;
use hwwasm::{
    ir,
    translate::{Target, Translator},
};
use hwwasm_aslp::parser;
use std::{fs, path::PathBuf};

#[derive(ClapParser)]
#[command(version, about)]
struct Args {
    /// Input file to be translated.
    file: PathBuf,

    /// Print debugging output (repeat for more detail)
    #[arg(short = 'd', long = "debug", action = clap::ArgAction::Count)]
    debug_level: u8,
}

fn main() -> Result<()> {
    let args = Args::parse();

    // Parse
    let src = fs::read_to_string(args.file)?;
    let block = parser::parse(&src)?;

    // Translate
    let mut translator = Translator::new();

    // sha1c q5, s6, v1.4s
    //
    // Args:
    //  hash_abcd  register: Qd
    //  hash_e  register: Sn
    //  wk  register: Vm.4S
    let z = Target::Var("_Z".to_string());
    translator.arg(ir::Type::Int(128), Target::Index(Box::new(z.clone()), 5));
    translator.arg(ir::Type::Int(32), Target::Index(Box::new(z.clone()), 6));
    translator.arg(ir::Type::Int(128), Target::Index(Box::new(z.clone()), 1));

    translator.translate(&block)?;

    // Print
    println!("{:#?}", translator.function());

    Ok(())
}
