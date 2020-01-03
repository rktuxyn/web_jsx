# web_jsx
web_jsx *.wjsx, *.wjsxh (Server-side Javascript) web extension handler (Run with IIS, Apache, Nginx)<br/>
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
const { pgSql, npgsql_db_type, npgsql_parameter_direction, npgsql_createParam } = require( "/addon/pgsql.js" );
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

/** Execute SCALAR statement**/
let params = [];
params.push( npgsql_createParam( "ct", npgsql_db_type.Jsonb, npgsql_parameter_direction.Input, { login_id: "system" } ) );
params.push( npgsql_createParam( "obj", npgsql_db_type.Jsonb, npgsql_parameter_direction.Input, { } ) );
params.push( npgsql_createParam( "ret_data_table", npgsql_db_type.Jsonb, npgsql_parameter_direction.Output ) );
params.push( npgsql_createParam( "ret_val", npgsql_db_type.Bigint, npgsql_parameter_direction.Output ) );
params.push( npgsql_createParam( "ret_msg", npgsql_db_type.Varchar, npgsql_parameter_direction.Output ) );
let res = _pgsql.execute_scalar( "data_storage.__get__historical_data", params );
context.response.write( res.ret_val );
context.response.write( res.ret_msg );
context.response.write( JSON.stringify( res.ret_data_table ) );
```
Send @Email with #web_jsx
```javascript
/* Import SMTP Addon*/
const { Smtp, MailMessage, mime_type, mime_encoder } = require( "/addon/smtp.js" );
/* Create SMTP instance*/
let smtp = new Smtp( "smtp://my_smtp.com", "smtp_user", "smtp_password" );
/* If you use CLI, you can enable debug mood*/
smtp.debug();
/* You may enable TLS mood */
smtp.enableTls();
/* If you enable TLS you need to add CERT */
smtp.cert( context.server_map_path( "/mycert.pem" ) );
/* Create MailMessage Instance */
let msg = new MailMessage( "from@address", "to@address" );
/* Add your mail subject */
msg.subject( "Your subject Test mail from web_jsx" );
/* Add your CC, BCC @address. You can add multiple address*/
msg.cc( "cc@address" ).bcc( "bcc@address" );
/* Add your mail body here and can set it body is html */
msg.body( `@body`,/*is html body*/ false );
/* Add your attachment. you can add multiple attachment here*/
msg.attachment( {
    name: "test",
    path: context.server_map_path( "test.zip" ),
    mime_type: mime_type.application.zip,
    encoder: mime_encoder.base64
} );
/* Send email here with MailMessage instance */
let rs = smtp.sendMail( msg );
/* Read your response, whether it was sent or failed..*/
/*{success:true|false, msg: reason}*/
context.response.write( JSON.stringify( rs ) );
```
OpenSSL EVP Symmetric Encryption and Decryption
```javascript
let
   key = "usQXLBd+CQptu6eC3xRaFg/P3Yxu3TiRIeImn9ehYSg="/* base64 key */,
   iv = "m4zSTQG1hkTgdH9yk6UW/nQLHobI"/* base64 IV */;
//Or you may generate rendom key and iv
const crypto_inf = crypto.generate_key_iv();
let plainText = "Hello world";
/** Encryption **/
let encText = crypto.encrypt( plainText, crypto_inf.key, crypto_inf.iv );
context.response.write( `Encrypted:${encText}` );

/** Decryption **/
let decText = crypto.decrypt( enctext, crypto_inf.key, crypto_inf.iv );
context.response.write( `Decrypted:${dectext}` );
```
Learn more https://web_jsx.safeonline.world/ and https://www.youtube.com/channel/UCsEq3IcHPLJGFseuINsYs_w
