//=====================================================================================================================================================//

const char restartPage[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>

<body>
    System is Restarting..........<br>
    Please wait 10 second to let system restart.
    <form action="/" method="GET">
        <button>BACK</button>
    </form>
</body>

</html>
)=====";

const char source[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        * {
            box-sizing: border-box;
        }
        
        body {
            background-color: #F4F7FC;
            margin: 0;
            padding: 0;
            font-family: "Source Sans Pro";
        }
        
        h1,
        h2,
        h3,
        h4,
        h5,
        h6 {
            margin: 0;
            padding: 0;
        }
        
        h1.brand {
            color: #505050;
        }
        
        .brand {
            width: 50%;
            float: left;
        }
        
        .ip {
            width: 50%;
            float: left;
            text-align: right;
        }
        
        h1.brand span,
        .ip {
            color: #999999;
            ;
        }
        
        h2,
        h4 {
            margin-bottom: 10px;
        }
        
        .container,
        .header {
            padding: 20px;
        }
        
        .header {
            background-color: #FFFFFF;
            border-bottom: 1px solid #EEEEEE;
            overflow: auto;
        }
        
        .panel {
            padding: 20px;
            background-color: #FFFFFF;
            margin-bottom: 20px;
            width: 50%;
        }
        
        .form-group {
            margin-bottom: 10px;
            overflow: auto;
        }
        /* form label{
        width: 20%;
        float: left;
    }
    form .input {
        width: 80%;
        float: left;
    } */
        
        form button {
            color: #FFFFFF;
            background-color: #333333;
            margin: 10px 0px 10px 0px;
            width: 150px;
            font-size: 15px;
            display: inline-block;
            margin-bottom: 0;
            font-weight: normal;
            text-align: center;
            white-space: nowrap;
            vertical-align: middle;
            -ms-touch-action: manipulation;
            touch-action: manipulation;
            cursor: pointer;
            background-image: none;
            border: 1px solid transparent;
            padding: 6px 12px;
            font-size: 14px;
            line-height: 1.42857143;
            border-radius: 4px;
            -webkit-user-select: none;
            -moz-user-select: none;
            -ms-user-select: none;
            user-select: none;
        }
        
        .form-control {
            display: block;
            width: 100%;
            height: 34px;
            padding: 6px 12px;
            font-size: 14px;
            line-height: 1.42857143;
            color: #555555;
            background-color: #fff;
            background-image: none;
            border: 1px solid #ccc;
            border-radius: 4px;
        }
    </style>
</head>

<body>
    <div class="header">
        <h1 class="brand">WODWORX <span>ACS</span></h1> <span class="ip">http://192.168.4.1/</span></div>
    <div class="container">
        <div class="content">
            <h2>SETUP</h2>
            <div class="panel">
                <h4>WIFI</h4>
                <form action="/submit" method="POST">
                    <div class="form-group">
                        <label>SSID</label>
                        <div class="input">
                            <input class="form-control" type="text" name="ssid" placeholder="ssid-holder         ">
                        </div>
                    </div>
                    <div class="form-group">
                        <label>PASSWORD</label>
                        <div class="input">
                            <input class="form-control" type="password" name="pswd" placeholder="password-holder     ">
                        </div>
                    </div>
                    <button type="submit">Submit</button>
                </form>

            </div>
            <div class="panel">
                <h4>DEVICE</h4>
                <form action="/submit_api" method="POST">
                    <div class="form-group">
                        <label>UNIQUE ID</label>
                        <div class="input">                                                  
                            <input class="form-control" type="text" name="uuid" placeholder="uuid-holder                     ">
                        </div>
                    </div>
                    <div class="form-group">
                        <label>API KEY</label>
                        <div class="input">
                            <input class="form-control" type="text" name="apiKey" placeholder="apikey-holder            ">
                        </div>
                    </div>
                    <button type="submit">Submit</button>
                </form>

            </div>
            <div class="panel">
                <form action="reset" method="GET">
                    <button type="submit">RESET</button>
                </form>
                <form action="/restart" method="GET">
                    <button type="submit">RESTART</button>
                </form>
            </div>
            <div class="panel">
                <h4>FILE UPLOAD</h4>
                <label>Please select the File to Upload</label>
                <form action="/" method="POST" enctype="multipart/form-data">
                    <input type="file" id="file" name="file" accept=".html">
                    <br>
                    <label>Please Enter the Password </label>
                    <input type="password" id="filepassword" name="filepassword" placeholder="password">
                    <br>
                    <button type="submit">Submit</button>
                </form>
            </div>
        </div>
</body>

</html>
)=====";

const char restart[] PROGMEM = R"=====(

)=====";
