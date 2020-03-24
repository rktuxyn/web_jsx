**HOW TO INSTALL WEB_JSX TO YOUR SYSTEM**<br/>

**web_jsx** *.wjsx & *.wjsxh **(Server-side Javascript)** FastCgi/Cgi (Runs with IIS, Apache, Nginx)<br/>

==> Download ```web_jsx``` current version from [here](https://github.com/safeonlineworld/web_jsx/releases).<br/>
==> Extract zip file to C drive.<br/>
==> Add to path environment variables (e.g. ```C:\web_jsx\```).<br/>
==> Bind ```web_jsx.exe``` with IIS/Apache/Nginx According to ```FastCgiModule```.<br/>
==> You may follow this [documentation](https://docs.microsoft.com/en-us/iis/application-frameworks/install-and-configure-php-applications-on-iis/using-fastcgi-to-host-php-applications-on-iis) and bind **web_jsx** with IIS.<br/>
==> See ```module.cfg``` to active/in-active C++ Native module. e.g. ```wj_npgsql_wrapper.dll```, ```wj_mysql_wrapper.dll```, ```wj_uws.dll```, ```wj_image.dll``` etc.<br/>
==> ```wj_core.dll``` (represent the ```web_jsx``` native context) is required for both of ```CLI``` and ```fcgi```<br/>

**Instructions to run ```web_jsx``` in Nginx(windows)**<br/>

**Install & Configure**<br/>

==> [Download](http://nginx.org/download/nginx-1.16.1.zip) Nginx for Windows.<br/>
==> Download attached ```nginx.config```, ```fastcgi_params```, ```run_fcgi.js``` files.<br/>
==> Replace original ```nginx.config``` & ```fastcgi_params```  files  located in config folder of Ngnix Root folder with downloaded ```nginx.config``` & ```fastcgi_params``` files.<br/>
==> Copy ```run_fcgi.js``` file anywhere.<br/>
==> Create your web server root folder in any drive example : ```D:\web_jsx_root```<br/>
==> Change your web server root folder directory in ```nginx.config``` file , look for the **```root D:/web_jsx_root```** line in ```nginx.config``` file & change the directory to your created directory address.<br/>
==> Change your Nginx root folder address in ```run_fcgi.js``` file look for the **``` process_path: "C:\\nginx\\nginx.exe"```** line in ```run_fcgi.js``` file & change the address with your Nginx root folder address.<br/>

**Run web_jsx with Nginx**<br/>

==> Create a ```index.jsxh``` or ```index.jsx``` file in your web server root folder, write something to test like ```context.response.write("Hello Nginx.."); ```<br/>
==> Open cmd ```run_fcgi.js``` folder .<br/>
==> Write this command to start **web_jsx** with Nginx : ```web_jsx run_fcgi.js```<br/>
==> You will see logs with opened port no & process ID.<br/>
==> Go to the browser & open ```localhost``` with port ```8080``` ex: ```http://localhost:8080/index.jsxh```<br/>

**Enjoy Server-side Javascript with web_jsx**<br/>

Note: **web_jsx v1.0.0.1** supported Windows x64 bit operating system.<br/>
Runtime Dependency : Microsoft Visual C++ 2015-2019 Redistributable 64bit Package. [Download](https://aka.ms/vs/16/release/vc_redist.x64.exe)
