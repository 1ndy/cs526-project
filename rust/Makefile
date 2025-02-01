
all: wasm wat

wat: wasm
	wasm2wat lib.wasm > lib.wat

wasm:
	rustc --crate-type cdylib \
		  --target=wasm32-unknown-unknown \
		  -C link-dead-code=true lib.rs

llvmir:
	rustc --crate-type cdylib \
		  --emit llvm-ir \
		  --target=wasm32-unknown-unknown \
		  -C link-dead-code=true lib.rs

clean:
	rm -f lib.wasm lib.wat lib.ll