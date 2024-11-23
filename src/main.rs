use anyhow::Result;
use clap::Parser as ClapParser;
use hwwasm::translate::Translator;
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
    let translator = Translator::new();
    translator.translate(&block);

    Ok(())
}
