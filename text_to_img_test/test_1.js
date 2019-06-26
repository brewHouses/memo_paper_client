const TextToSVG = require('text-to-svg');
const textToSVG = TextToSVG.loadSync();
 
const attributes = {fill: 'red', stroke: 'black'};
const options = {x: 0, y: 0, fontSize: 72, anchor: 'top', attributes: attributes};
 
const svg = textToSVG.getSVG('y4e213', options);
 

var fs = require('fs');
var svg2img = require('svg2img');
var btoa = require('btoa');

var svgString = svg
//1. convert from svg string
svg2img(svgString, {'width':400, 'height':300} , function(error, buffer) {
    //returns a Buffer
    fs.writeFileSync('foo1.png', buffer);
});

