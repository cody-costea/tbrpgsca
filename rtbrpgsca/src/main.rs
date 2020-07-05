
#[macro_use]
mod role;
mod actor;
mod ability;
mod costume;
mod library;
mod state;
mod scene;
mod play;

extern crate sciter;

fn main() {
    println!("Hello, world!!!");
    let mut frame = sciter::Window::new();
    frame.load_file("main.htm");
    frame.run_app();
}
