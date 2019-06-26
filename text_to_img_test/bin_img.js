var floydSteinberg = require('floyd-steinberg');
var fs = require('fs');
var PNG = require('pngjs').PNG;

fs.createReadStream('a.png').pipe(new PNG()).on('parsed', function() {
  floydSteinberg(this).pack().pipe(fs.createWriteStream('b.png'));
});

var Jimp = require('jimp');


Jimp.read('./b.jpg')
  .then(lenna => {
    return lenna
      .resize(400,300)
      .quality(60) // set JPEG quality
      .greyscale() // set greyscale
      .write('b1.png', to_bin); // save
  })
  .catch(err => {
    console.error(err);
  });

var to_bin =()=> {
fs.createReadStream('b1.png').pipe(new PNG()).on('parsed', function() {
  floydSteinberg(this).pack().pipe(fs.createWriteStream('b2.png'));
});
}



var ascii = ['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p']



//function handle_bin_img_with_res(img)

var http = require('http');
var url = require('url');
var util = require('util');
var xxxx = 0
http.createServer(function(req, res){
  function handle_bin_img(img){
      img = img.bitmap.data;
      length = img.length
      var ret = ""
      temp_index = 1
      temp = 0
      for(let i=3; i < length; i+=4){
          if(img[i] == 0)
              temp += Math.pow(2, 3-(temp_index-1))
          if(temp_index % 4 == 0){
              temp_index = 1;
              ret += ascii[temp]
              temp = 0
              continue
          }
          temp_index++
      }
      console.log('ret')
      if(xxxx % 2 == 0){
        console.log('a');
        res.end(ret.substring(0,15000));
      }
      else {
        console.log('b');
        res.end(ret.substring(15000,30000));
      }
      xxxx++;
      return ret;
  }
    res.writeHead(200, {'Content-Type': 'text/plain; charset=utf-8'});
    Jimp.read('./b2.png').then(handle_bin_img)
    //res.end(util.inspect(url.parse(req.url, true)));
}).listen(3000);
