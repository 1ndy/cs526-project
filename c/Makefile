
all: wasm wat

wat: wasm
	wasm2wat lib.wasm > lib.wat

wasm:
	emcc -c -o lib.wasm lib.c

llvmir:
	clang -S -emit-llvm -c lib.c

clean:
	rm -f lib.wasm lib.wat lib.ll