var express = require('express');
var router = express.Router();
var server = require('../server.js');
var request = require('request');


router.get('/', function (req, res, next) {
    res.render('board');
})

router.get('/board', function (req, res, next) {
    res.render('board');
})

router.get('/streams', function (req, res, next) {

    res.render('streams');
})

router.get('/stats', function (req, res, next) {
    res.render('stats');
})

router.get('/init', function(req, res, next) {
    console.log('called init func');
    // wait for LLAPI before sending response??
    setTimeout(() => {res.send(false)}, 2000) // result
})

module.exports = router