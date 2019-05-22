var express = require('express');
var compression = require('compression');
const winston = require('winston');
var bodyparser = require('body-parser');
var cookieParser = require('cookie-parser');
var path = require('path');
var request = require('request');
const tsFormat = () => (new Date()).toLocaleTimeString()
const logger = new (winston.Logger)({
  transports: [
    new (winston.transports.Console)({
      timestamp: tsFormat,
      colorize: true
    })
  ]
})

var index = require('./routes/index');
var board_state = { 
   is_init: false,
   software_version: 0,
   firmware_version: 0
};

var properties = {
  ip: '0.0.0.0',
  port: '7777',
}
var SERVER_URL = 'http://0.0.0.0:8081/v1.0';

function server(port, ip) {
   var app = express();
   var server = null;
   this.init = () => {
      app.use(compression())
      app.use(bodyparser.json());
      app.use(bodyparser.urlencoded({ extended: false }));
      app.use(cookieParser());
      app.use(express.static(path.join(__dirname, 'public')));
  
      app.set('views', path.join(__dirname, 'views'));
      app.set('view engine', 'pug');
  
      app.use('/', index);
      app.use(function (err, req, res, next) {
        if (err.status) {
          res.status(err.status).json({
            code: err.status,
            error: (err.message) ? err.message : 'Internal server error. No message available.',
            debug: (err.stack) ? err.stack : 'No stack available.'
          });
        } else {
          res.status(500).json({
            code: 500,
            error: (err.message) ? err.message : 'Internal server error. No message available.',
            debug: (err.stack) ? err.stack : 'No stack available.'
          });
        }
      })
  
      app.use(function (req, res, next) {
        res.status(404).json({
          code: 404,
          error: `Could not find the requested resource '${req.path}'.`,
          debug: req.path
        })
      })
  
      return this
    }
  
    this.start = (cb) => {
      server = app.listen(port, ip, function (e) {
        var host = server.address().address;
        var port = server.address().port;
        if (e) {
          if (e.code === 'EADDRINUSE') {
            logger.fatal('Address http://%s:%s already in use.', host, port)
            server.close()
          };
          if (cb) cb(e)
        } else {
          // if (!board_state.isInit) {
          //   request.post(SERVER_URL + "/init", (error, response, body) => {
          //     if (error || response.statusCode == 500) {
          //       logger.fatal("Hardware initalization failure. Logging off.");
          //       process.exit();
          //     }
          //   })
          //   request.get(SERVER_URL + "/status", (error, response, body) => {
          //     if (error || response.statusCode !== 200) {
          //       logger.fatal("Incompatible LL API modules. Logging off.");
          //       process.exit();
          //     } else {
          //       board_state.software_version = body.software_version;
          //       board_state.firmware_version = body.firmware_version;
          //     } 
          //   })
          //   board_state.is_init = true;
          // }
          logger.info('ProAV web application is running at http://%s:%s', host, port)

          if (cb) cb()
        };
      })
      return this
    }
}
 
 process.on('SIGINT', () => {
   console.log(process.memoryUsage().rss);
   process.exit()
 });

process.on('uncaughtException', (err) => {
   logger.error('Uncaught exception!')
   console.log(err)
 })
 
 module.exports.server = server
 module.exports.board_state = board_state
 module.exports.SERVER_URL = SERVER_URL

