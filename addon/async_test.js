let example = async () => {
    await async_func( ( a ) => {
        __print( a );
    } );
};
example();
async function main() {
	try {
		return new Promise( ( resolve, reject ) => {
			//resolve( 'First call' );
			resolve( 'Swallowed resolve' );
			//reject( new Error( 'Swallowed reject' ) );
		} );
	} catch {
		throw new Error( 'Failed' );
	}
}
main().then( console.log );

console.log("Hello");