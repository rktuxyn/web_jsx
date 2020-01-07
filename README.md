# web_jsx
web_jsx *.wjsx, *.wjsxh (Server-side Javascript) web extension handler (Run with IIS, Apache, Nginx)<br/>
<br/>
Read POST data
```javascript
let payload = "";
context.request.read_payload( ( buff, len ) => {
	payload += buff;
} );
```
Or
```javascript
let http_payload = context.request.read_posted_file( context.request.content_length, context.request.content_type );
if ( http_payload.is_multipart() ){
    let temp_dir = context.root_dir + "/temp/";
    sys.create_directory( "/temp/" );
    let rs = http_payload.read_all( temp_dir );
	let count = 1;
    context.response.write( "<ul>" );
	let upload_dir = context.root_dir + "/upload/";
    sys.create_directory( "/upload/" );
    try {
        http_payload.read_files( ( file ) => {
            try {
                context.response.write( `<li>SL:${count}</li>` );
                context.response.write( `<li>content_type:${file.get_content_type()}</li>` );
                context.response.write( `<li>name:${file.get_name()}</li>` );
                context.response.write( `<li>file_name:${file.get_file_name()}</li>` );
                context.response.write( `<li>content_disposition:${file.get_content_disposition()}</li>` );
                context.response.write( `<li>file Size: ${parseFloat( file.get_file_size() / 1024 ).toFixed( 4 )} Kb</li>` );
                context.response.write( `<li>Writing file ${file.get_file_name()}</li>` );
                file.save_as( `${upload_dir}${file.get_file_name()}` );
            } catch ( e ) {
                context.response.write( `<li style="color:red;">Error:${e.message}</li>` );
            }
            count++;
        } );
    } catch ( e ) {
        context.response.write( `<li style="color:red;">Error:${e.message}</li>` );
    }
    context.response.write( "</ul>" );
    context.response.write( `Total ${rs} file(s) saved...` );
    //Or Save all file to $upload_dir
    http_payload.save_to_file( upload_dir );
	
} else {
	http_payload.read_line( ( buff, len ) => {
		context.response.write( buff );
	} );
}
http_payload.release();
```
Write payload to file<br/>
```javascript
let temp_dir = context.root_dir + "/temp/";
sys.create_directory( "/temp/" );
let c = context.request.write_file_from_payload( temp_dir );
```
#Content-Encoding Gzip response 
```javascript
context.response.as_gzip()
```
This compression write directly outstream
```javascript
//If you like to response compressed file
let compress = new gzip.compress();
context.response.header( "Content-Type", "image/jpeg" );
compress.flush_header();
compress.write_from_file( "/images/web_jsx.jpg", gzip.Z_FINISH );
compress.flush();
//Or If you like to response compressed string
let compress = new gzip.compress();
context.response.header( "Content-Type", "text/plain" );
compress.flush_header();
compress.write( `Welcome to`, gzip.Z_NO_FLUSH );
compress.write( `WebJsx`, gzip.Z_NO_FLUSH );
compress.write_from_file( "context.json", gzip.Z_NO_FLUSH );
compress.write( `You should write finsh stream`, gzip.Z_FINISH );
compress.flush();
```
Direct write to outstream
```javascript
let cout = new stdout();
context.response.header( "Content-Type", "text/plain" );
cout.flush_header();
cout.write( "Hello world...\r\n" );
//Or you may write file directly outstream
cout.write_from_file( "context.json" );
cout.flush();
```
Connect WebJsx with PostgreSQL
```javascript
//Read data from Postgres SQL
let resp = npgsql.execute_io("Server=localhost; Port=5432; UserId=postgres;Password=1##$1@6Z;Database=sow; keepalive=10; CommandTimeout=100000;", 
	"__sql_execute", JSON.stringify( {} ), JSON.stringify( {
		"sql":"select * from jsx.community"
	} )
);
__print(JSON.stringify(resp));
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
//Or Create new Instance and execute multiple quary
let pgsql = new PgSql();
//Connect database once and execute multiple quary
pgsql.connect( cfg.database.db_conn );
//Execute stored procedure
let resp = pgsql.execute_io(
	"aut.user_info",
	JSON.stringify( {} ),
	JSON.stringify( {} )
);
//Release all Active Connection and release all resource
pgsql.exit_nicely();
```
Connect WebJsx with MySQL
```javascript
let mysql = new MySql();
mysql.connect( {
	host: "localhost",
	database: "web_jsx_db",
	user: "root",
	password: "mysql123",
	port: 0
} );
//Drop Database
mysql.exec( 'DROP DATABASE IF EXISTS web_jsx_db_2' );
//Create Database
mysql.exec( 'CREATE DATABASE IF NOT EXISTS web_jsx_db_2' );
//Switch Database
mysql.switch_database("web_jsx_db_2");
//Create TABLE
mysql.exec('CREATE TABLE IF NOT EXISTS Persons ( PersonID int,LastName varchar(255),FirstName varchar(255),Address varchar(255), City varchar(255))');
//Truncate TABLE
mysql.exec( "truncate table Persons" );
//Execute Insert statement
mysql.exec( "INSERT INTO Persons (`PersonID`, `LastName`, `FirstName`, `Address`, `City`)VALUES (11, 'Rajib', 'Chy', 'Panchlaish, katalgong', 'Chittagong');" );
//Execute multiple row select statement
mysql.execute_query( "select * from Persons", ( i, row ) => {
	print( row );
} );
//Execute 1 cell select statement
let address = mysql.exec('select Address from Persons limit 1');
//Release all Active Connection and release all resource
mysql.clear();
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
