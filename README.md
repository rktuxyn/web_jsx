# web_jsx
web_jsx *.jsx, *.jsxh (back-end Javascript) web extension handler (Run with IIS, Apache, Nginx)<br/>
<br/>
Read POST data
```javascript
let payload = "";
context.request.read_payload( function ( buff, count ) {
	payload += buff;
} );
```
Write payload to file<br/>
```javascript
let c = fs.write_file_from_payload( "post_data.txt" );
```
#Content-Encoding Gzip response 
```javascript
context.response.as_gzip()
```
Read data from Postgres SQL<br/>
```javascript
let resp = npgsql.execute_io("Server=localhost; Port=5432; UserId=postgres;Password=1##$1@6Z;Database=sow; keepalive=10; CommandTimeout=100000;", 
	"__sql_execute", JSON.stringify( {} ), JSON.stringify( {
		"sql":"select * from jsx.community"
	} )
);
__print(JSON.stringify(resp));
```
Implement module with require
```javascript
/** read web config module **/
let cfg = require( "/addon/web.conf.js" );
/** get pgsql addon **/
const { pgSql } = require( "/addon/pgsql.js" );
/** initialize pgSql instance **/
let _pgsql = new pgSql( cfg.database.db_conn, JSON.stringify( { login_id: "system" } ) );

context.response.write( '<table><tbody>' );
/** Read row(s) from PostgreSQL with plain text quary **/
let res = _pgsql.data_reader( "select * from auth.login where login_id ={0}", ["rajibs"], ( i, row ) => {
    let _row = JSON.parse( row );
    context.response.write( "<tr>");
    _row.forEach( ( val ) => {
        context.response.write( `<td>${val}</td>` );
    } );
    context.response.write( "</tr>" );
} );
context.response.write( '</tbody></table>' );

/** Execute INSERT statement and get last sequence number**/
let designation_sid = _pgsql.execute_query( "INSERT INTO c_type.designation(designation_id, title)VALUES ({0},{1}) returning designation_sid", ["NO_ADMIN","NO_DSG"] );

/** Execute DELETE statement**/
_pgsql.execute_query( "delete from c_type.designation where designation_sid > {0}  and designation_sid not in({1})", [2, designation_sid] );
```
Learn more https://web_jsx.safeonline.world/ and https://www.youtube.com/channel/UCsEq3IcHPLJGFseuINsYs_w
