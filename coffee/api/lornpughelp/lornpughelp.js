#!/usr/bin/env node
(function webpackUniversalModuleDefinition(root, factory) {
	if(typeof exports === 'object' && typeof module === 'object')
		module.exports = factory();
	else if(typeof define === 'function' && define.amd)
		define([], factory);
	else if(typeof exports === 'object')
		exports["mlib"] = factory();
	else
		root["mlib"] = factory();
})(global, function() {
return (() => {
var exports = {};
exports.id = "lornpughelp";
exports.ids = ["lornpughelp"];
exports.modules = {

/***/ "./src/lornpughelp.cs":
/*!****************************!*
  !*** ./src/lornpughelp.cs ***!
  \****************************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "@babel/runtime/helpers/interopRequireDefault");

Object.defineProperty(exports, "__esModule", ({
  value: true
}));
exports.default = exports.dodir = exports.dolp = exports.iscmd = void 0;

var _regenerator = _interopRequireDefault(__webpack_require__(/*! @babel/runtime/regenerator */ "@babel/runtime/regenerator"));

var _asyncToGenerator2 = _interopRequireDefault(__webpack_require__(/*! @babel/runtime/helpers/asyncToGenerator */ "@babel/runtime/helpers/asyncToGenerator"));

var _chalk = _interopRequireDefault(__webpack_require__(/*! chalk */ "chalk"));

var _mlib = __webpack_require__(/*! mlib */ "./src/mlib/src/index.cs");

var dir1file;

// -----------------pure function-----------------
// 判断是否有命令参数(--开始的命令参数), 没有--的都是文件
var iscmd = function iscmd(x) {
  return /^--/.test(x);
}; // 处理lp命令


exports.iscmd = iscmd;

var dolp = /*#__PURE__*/function () {
  var _ref = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee() {
    var ok, reg, str;
    return _regenerator["default"].wrap(function _callee$(_context) {
      while (1) {
        switch (_context.prev = _context.next) {
          case 0:
            str = "# \u8214\u72D7\u9879\u76EE\nalias lp=\"lpg\"";
            reg = /alias\s*lp="lpg"/;
            _context.next = 4;
            return _mlib.file.uniqueadd({
              reg: reg,
              str: str,
              path: '~/.zshrc'
            });

          case 4:
            ok = _context.sent;

            if (!ok) {
              _context.next = 13;
              break;
            }

            console.log('命令设置为lp', _chalk["default"].greenBright('成功: '), '需要刷新配置才会生效, 推荐使用快捷重启命令:');
            console.log(_chalk["default"].greenBright('exec zsh'), _chalk["default"].yellow('# 重启当前terminal'));
            console.log('如果不能重启terminal: ');
            console.log(_chalk["default"].greenBright('source ~/.zshrc'), _chalk["default"].yellow('# 导入配置文件方法一'));
            return _context.abrupt("return", console.log(_chalk["default"].greenBright('. ~/.zshrc'), _chalk["default"].yellow('# 导入配置文件方法二')));

          case 13:
            return _context.abrupt("return", console.log(_chalk["default"].redBright('没有'), '进行设置, 可能是配置文件已经存在', _chalk["default"].yellow('请手工检查文件: '), '~/.zshrc'));

          case 14:
          case "end":
            return _context.stop();
        }
      }
    }, _callee);
  }));

  return function dolp() {
    return _ref.apply(this, arguments);
  };
}(); // 处理一个cs或者coffee的初始化


exports.dolp = dolp;

dir1file = /*#__PURE__*/function () {
  var _ref2 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee2(path) {
    var p;
    return _regenerator["default"].wrap(function _callee2$(_context2) {
      while (1) {
        switch (_context2.prev = _context2.next) {
          case 0:
            p = path + '.lp'; // 判断对应目录是否存在.

            if (!_mlib.file.exist(p)) {
              _context2.next = 3;
              break;
            }

            return _context2.abrupt("return");

          case 3:
            _context2.next = 5;
            return _mlib.file.mkdir(p);

          case 5:
            _context2.next = 7;
            return _mlib.file.cover({
              path: p + '/aaa.0i.cson',
              str: ''
            });

          case 7:
            _context2.next = 9;
            return _mlib.file.cover({
              path: p + '/aaa.0o.cson',
              str: ''
            });

          case 9:
            _context2.next = 11;
            return _mlib.file.cover({
              path: p + '/cto.cson',
              str: ''
            });

          case 11:
            return _context2.abrupt("return", _context2.sent);

          case 12:
          case "end":
            return _context2.stop();
        }
      }
    }, _callee2);
  }));

  return function dir1file(_x) {
    return _ref2.apply(this, arguments);
  };
}(); // 处理初始化测试dir


var dodir = /*#__PURE__*/function () {
  var _ref3 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee3(path) {
    var i, len, ref, stat, x;
    return _regenerator["default"].wrap(function _callee3$(_context3) {
      while (1) {
        switch (_context3.prev = _context3.next) {
          case 0:
            _context3.next = 2;
            return _mlib.file.filetype(path);

          case 2:
            stat = _context3.sent;

            if (!stat.isfile) {
              _context3.next = 7;
              break;
            }

            if (/\.coffee$|\.cs$/.test(path)) {
              _context3.next = 6;
              break;
            }

            return _context3.abrupt("return");

          case 6:
            dir1file(path);

          case 7:
            if (!stat.isdir) {
              _context3.next = 14;
              break;
            }

            if (!/\./.test(path)) {
              _context3.next = 10;
              break;
            }

            return _context3.abrupt("return");

          case 10:
            _context3.next = 12;
            return _mlib.file.readir(path);

          case 12:
            ref = _context3.sent;

            // 除了file和dir可能还有其他情况, 比如符号链接
            for (i = 0, len = ref.length; i < len; i++) {
              x = ref[i]; // 读取目录, 逐项处理

              dodir(x);
            }

          case 14:
          case "end":
            return _context3.stop();
        }
      }
    }, _callee3);
  }));

  return function dodir(_x2) {
    return _ref3.apply(this, arguments);
  };
}();

exports.dodir = dodir;
var _default = {
  iscmd: iscmd,
  dolp: dolp,
  dodir: dodir
};
exports.default = _default;
dodir('.');

/***/ }),

/***/ "./src/mlib/src/color.cs":
/*!*******************************!*
  !*** ./src/mlib/src/color.cs ***!
  \*******************************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "@babel/runtime/helpers/interopRequireDefault");

Object.defineProperty(exports, "__esModule", ({
  value: true
}));
exports.default = exports.samergbstr = exports.getdark = exports.getlight = exports.lrgb = exports.lstr = exports.rgbok = void 0;

var _colorname = _interopRequireDefault(__webpack_require__(/*! ./colorname.cson */ "./src/mlib/src/colorname.cson"));

/*
 * 关于颜色的库, 这个库会被重写, 因为hsl的亮度判断可以认为是错的. 所以可以保留hsl相关的函数, 但是, 实际应用函数需要重写.
 *
 */

/**
	* 拿到视觉暗度
	* 用来判断颜色是否够暗
	*
 */

/**
	* 拿到视觉亮度
	* 用来判断颜色是否够亮
	* 未来用荧光体发光能量来计算数值.
 */

/*
 * 从16进制到rgb
 *  #fc2  #fc29 #fc292923 #fc2929, 3,4,6,8都有可能
 */

/**
	 * lightrgb核心亮度处理函数
	 *  * 亮度处理逻辑 t=max(100-g, 200-r), t+r, t+g, t+b
	 *  * 暗度处理逻辑 t=max(g-60, r-100, b-150), r-t, g-t, b-t
	 * 废弃return false并不合理
 */

/**
 * 从str到调亮的颜色str
 *  这里处理一个style string的亮度
 * 纯函数
 * str: 一个颜色字符串
 * isback: 是否为背景
 * 废弃 return false 并不合理
 */

/*
* 从rgb到str
* */

/**
* 3种情况
* 1. rgb情况: 从"rgb(154, 154, 0)"转化为: {r:154,g:154,b:0,}
* "rgba(0, 0, 0, 0)" 还需兼容此种情况
* 2. 16进制情况:  #fc2  #fc29 #fc292923 #fc2929
* 3. 颜色名: white, black, organge
* */
var darkcolor, getrgbdark, getrgblight, hex2rgb, lightcolor, lightrgb, lightstr, mlight, rgb2str, str2rgb, test;
darkcolor = 'rgba(44,44,44,1)'; //dark的基础颜色

lightcolor = 'rgba(200,200,200,1)';
/**
* ---------------------单独处理前景或者背景, 配合darkcss的思路---------------------------
* 亮度判断逻辑 g>100, r>200, 或者g+r>300
* 亮度处理逻辑 t=max(100-g, 200-r), t+r, t+g, t+b
*
* 暗度判断逻辑 g<60 r<100 b<150
* 暗度处理逻辑 t=max(g-60, r-100, b-150), r-t, g-t, b-t
*
 */

/**
* rgbok
* 判断颜色是否需要处理
*  * 亮度判断逻辑 g>100, r>200, 或者g+r>300
*  * 暗度判断逻辑 g<60 r<100 b<150
* 参数
* 	rgb:{r,g,b}
*  isback: 是否为背景图片
* 返回值
* 		true: 不需要处理
* 		false: 需要处理
 */

var rgbok = function rgbok(_ref) {
  var rgb = _ref.rgb,
      _ref$isback = _ref.isback,
      isback = _ref$isback === void 0 ? true : _ref$isback;
  var b, g, r;
  r = rgb.r;
  g = rgb.g;
  b = rgb.b;

  if (isback) {
    return g < 60 && r < 100 && b < 150;
  } else {
    return r + g > 300;
  }
}; //暗度判断
//光明模式, 如果亮度<0.7则需要处理, 否则就不需要处理

/**
 * 从str到调亮的颜色str
 *  这里处理一个style string的亮度
 * 纯函数
 * str: 一个颜色字符串
 * isback: 是否为背景
 * 废弃 return false 并不合理
 */


exports.rgbok = rgbok;

var lstr = function lstr(_ref2) {
  var str = _ref2.str,
      _ref2$isback = _ref2.isback,
      isback = _ref2$isback === void 0 ? true : _ref2$isback;
  var l;

  if (!str) {
    return false;
  }

  if (str === "initial") {
    //如果是initial, 那么直接用我的值.
    return isback ? darkcolor : lightcolor;
  }

  if (/var\(--/.test(str)) {
    //fixme 暂时在这里忽略css var  #todo
    return isback ? darkcolor : lightcolor;
  }

  l = lrgb({
    rgb: str2rgb(str),
    isback: isback
  });
  return rgb2str(l);
};
/**
	 * lightrgb核心亮度处理函数
	 *  * 亮度处理逻辑 t=max(100-g, 200-r), t+r, t+g, t+b
	 *  * 暗度处理逻辑 t=max(g-60, r-100, b-150), r-t, g-t, b-t
	 * 废弃return false并不合理
 */


exports.lstr = lstr;

var lrgb = function lrgb(_ref3) {
  var rgb = _ref3.rgb,
      _ref3$isback = _ref3.isback,
      isback = _ref3$isback === void 0 ? true : _ref3$isback;
  var b, g, r, t;

  if (rgbok({
    rgb: rgb,
    isback: isback
  })) {
    return rgb;
  }

  r = rgb.r;
  g = rgb.g;
  b = rgb.b;

  if (isback) {
    //处理暗黑背景
    t = Math.max(g - 60, r - 100, b - 150);
    r -= t;
    g -= t;
    b -= t; //处理明亮前景
  } else {
    t = Math.max(100 - g, 200 - r);
    r += t;
    g += t;
    b += t;
  }

  return {
    r: r,
    g: g,
    b: b
  };
};

exports.lrgb = lrgb;

lightstr = function lightstr(_ref4) {
  var str = _ref4.str,
      _ref4$isback = _ref4.isback,
      isback = _ref4$isback === void 0 ? true : _ref4$isback;
  var l;

  if (!str) {
    return false;
  }

  if (str === "initial") {
    //如果是initial, 那么直接用我的值.
    return isback ? darkcolor : lightcolor;
  }

  if (/var\(--/.test(str)) {
    //fixme 暂时在这里忽略css var  #todo
    return isback ? darkcolor : lightcolor;
  }

  l = lightrgb({
    rgb: str2rgb(str),
    isback: isback
  });

  if (l) {
    return rgb2str(l);
  } else {
    return false;
  }
};

lightrgb = function lightrgb(_ref5) {
  var rgb = _ref5.rgb,
      _ref5$isback = _ref5.isback,
      isback = _ref5$isback === void 0 ? true : _ref5$isback;
  var b, g, r, t;

  if (rgbok({
    rgb: rgb,
    isback: isback
  })) {
    return false;
  }

  r = rgb.r;
  g = rgb.g;
  b = rgb.b;

  if (isback) {
    //处理暗黑背景
    t = Math.max(g - 60, r - 100, b - 150);
    r -= t;
    g -= t;
    b -= t; //处理明亮前景
  } else {
    t = Math.max(100 - g, 200 - r);
    r += t;
    g += t;
    b += t;
  }

  return {
    r: r,
    g: g,
    b: b
  };
};
/**
 * --------------对比度逻辑, 配合darkstyle-----------------
 * 这里的计算未必合适.
 */

/**
* 从str到亮度
 */


var getlight = function getlight(str) {
  return getrgblight(str2rgb(str));
};

exports.getlight = getlight;

var getdark = function getdark(str) {
  return getrgbdark(str2rgb(str));
};

exports.getdark = getdark;

getrgblight = function getrgblight(_ref6) {
  var r = _ref6.r,
      g = _ref6.g,
      b = _ref6.b;
  r = r * 1.2;
  g = g * 1.7;
  return mlight({
    r: r,
    g: g,
    b: b
  }).light;
};

getrgbdark = function getrgbdark(_ref7) {
  var r = _ref7.r,
      g = _ref7.g,
      b = _ref7.b;
  r = r * 1.3;
  g = g * 1.2;
  return mlight({
    r: r,
    g: g,
    b: b
  }).light;
};

mlight = function mlight(_ref8) {
  var r = _ref8.r,
      g = _ref8.g,
      b = _ref8.b;
  var b1, b2, b3;

  if (r > g) {
    b1 = r;
    b2 = g;
  } else {
    b1 = g;
    b2 = r;
  }

  if (b < b2) {
    b3 = b;
  } else if (b < b1) {
    b3 = b2;
    b2 = b;
  } else {
    b3 = b2;
    b2 = b1;
    b1 = b;
  }

  return {
    // 这里很不严谨, 亮度的心理阈限测量或许能让这个计算更精确.
    light: (b1 + b2 / 6) * 2 / 3,
    max: [b1, b2, b3]
  };
};
/*
	* 基础工具函数-------------------------------------------------------
	* 判断两个rgbstr实际相等
 */


var samergbstr = function samergbstr(rgbstr1, rgbstr2) {
  var b1, b2, g1, g2, r1, r2;

  var _str2rgb = str2rgb(rgbstr1);

  r1 = _str2rgb.r;
  g1 = _str2rgb.g;
  b1 = _str2rgb.b;

  var _str2rgb2 = str2rgb(rgbstr2);

  r2 = _str2rgb2.r;
  g2 = _str2rgb2.g;
  b2 = _str2rgb2.b;
  return r1 === r2 && g1 === g2 && b1 === b2;
};

exports.samergbstr = samergbstr;

str2rgb = function str2rgb(str) {
  var a, c, reg; //"rgb(154, 154, 0)"

  reg = /rgba?\(([0-9]{1,3}), ?([0-9]{1,3}), ?([0-9]{1,3})(, ?([0-9.%]+))?\)/i;
  c = reg.exec(str); //16进制颜色

  if (c == null) {
    c = hex2rgb(str);
  } //再判断名字颜色:


  if (c == null) {
    c = _colorname["default"][str] && hex2rgb(hex);
  }

  if (c == null) {
    return false;
  } //透明度


  a = c[5] != null ? c[5] : 1;
  return {
    r: +c[1],
    g: +c[2],
    b: +c[3],
    a: a //这里用+是为了把字符串转为数字, 不然会发生很隐蔽的bug.

  };
};

rgb2str = function rgb2str(_ref9) {
  var r = _ref9.r,
      g = _ref9.g,
      b = _ref9.b,
      _ref9$a = _ref9.a,
      a = _ref9$a === void 0 ? 1 : _ref9$a;

  if (a > 0.99) {
    return "rgb(" + r + "," + g + "," + b + ")";
  } else {
    return "rgba(" + r + "," + g + "," + b + "," + a + ")";
  }
};

hex2rgb = function hex2rgb(str) {
  var c, reg, x, y;
  reg = /#([^#]*)/i; //判断#62fa89这种类似方式的颜色值

  c = reg.exec(str);

  if ((c != null ? c[1] : void 0) == null) {
    return false;
  }

  x = c[1];
  y = [];

  switch (x.length) {
    case 3:
      y[1] = parseInt('' + x[0] + x[0], 16);
      y[2] = parseInt('' + x[1] + x[1], 16);
      y[3] = parseInt('' + x[2] + x[2], 16);
      break;

    case 4:
      //y[1]=parseInt(x[0],16);  /20201028修正错误, 这个会把fff转化为 r15g15b15, 其实应该是r255g255b255
      y[1] = parseInt('' + x[0] + x[0], 16);
      y[2] = parseInt('' + x[1] + x[1], 16);
      y[3] = parseInt('' + x[2] + x[2], 16);
      y[5] = parseInt(x[3], 16) / 255;
      break;

    case 6:
      y[1] = parseInt('' + x[0] + x[1], 16);
      y[2] = parseInt('' + x[2] + x[3], 16);
      y[3] = parseInt('' + x[4] + x[5], 16);
      break;

    case 8:
      y[1] = parseInt('' + x[0] + x[1], 16);
      y[2] = parseInt('' + x[2] + x[3], 16);
      y[3] = parseInt('' + x[4] + x[5], 16);
      y[5] = parseInt('' + x[6] + x[7], 16) / 255;
      break;

    default:
      //不满足上面四个情况, 直接返回false
      return false;
  }

  return y;
}; //console.log(str2rgb('white')); #test


test = function test() {
  var result;
  result = window.luoclr.lightstr({
    str: 'white',
    isback: true
  });
  return console.log({
    result: result
  });
}; //lightstr
//lightrgb


var _default = {
  lstr: lstr,
  lrgb: lrgb,
  getlight: getlight,
  getdark: getdark,
  samergbstr: samergbstr
};
exports.default = _default;

/***/ }),

/***/ "./src/mlib/src/file.cs":
/*!******************************!*
  !*** ./src/mlib/src/file.cs ***!
  \******************************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "@babel/runtime/helpers/interopRequireDefault");

Object.defineProperty(exports, "__esModule", ({
  value: true
}));
exports.default = exports.allfile = exports.readfile = exports.readir = exports.newdir = exports.mkdir = exports.exist = exports.filetype = exports.uniqueadd = exports.cover = exports.realpath = exports.savefile = void 0;

var _regenerator = _interopRequireDefault(__webpack_require__(/*! @babel/runtime/regenerator */ "@babel/runtime/regenerator"));

var _asyncToGenerator2 = _interopRequireDefault(__webpack_require__(/*! @babel/runtime/helpers/asyncToGenerator */ "@babel/runtime/helpers/asyncToGenerator"));

var _promises = _interopRequireDefault(__webpack_require__(/*! fs/promises */ "fs/promises"));

var _fs = _interopRequireDefault(__webpack_require__(/*! fs */ "fs"));

var _os = _interopRequireDefault(__webpack_require__(/*! os */ "os"));

//import {log} from './log.js'
var home, log;
home = _os["default"].homedir();
log = console.log; // todo 保存文件

var savefile = function savefile(_ref) {
  var json = _ref.json,
      _ref$filename = _ref.filename,
      filename = _ref$filename === void 0 ? "temptest.txt" : _ref$filename;
  return _promises["default"].writeFile(filename, JSON.stringify(json), function (err) {
    log('文件写入', filename);

    if (err) {
      return log(err);
    }
  });
}; // 拿到真正的path


exports.savefile = savefile;

var realpath = function realpath(path) {
  // home文件夹~, node不认, 要调整为os.homedir
  if (/^~/.test(path)) {
    return path.replace(/^~/, home);
  } else {
    return path;
  }
}; // 加文件内容, 覆盖式


exports.realpath = realpath;

var cover = /*#__PURE__*/function () {
  var _ref3 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee(_ref2) {
    var path, str, _ref2$options, options;

    return _regenerator["default"].wrap(function _callee$(_context) {
      while (1) {
        switch (_context.prev = _context.next) {
          case 0:
            path = _ref2.path, str = _ref2.str, _ref2$options = _ref2.options, options = _ref2$options === void 0 ? {
              encoding: 'utf8'
            } : _ref2$options;
            _context.next = 3;
            return _promises["default"].writeFile(realpath(path), str, options);

          case 3:
            return _context.abrupt("return", true);

          case 4:
          case "end":
            return _context.stop();
        }
      }
    }, _callee);
  }));

  return function cover(_x) {
    return _ref3.apply(this, arguments);
  };
}(); // 追加文件内容, 保证追加内容不重复


exports.cover = cover;

var uniqueadd = /*#__PURE__*/function () {
  var _ref5 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee2(_ref4) {
    var path, reg, str, _ref4$options, options, fstr, rpath;

    return _regenerator["default"].wrap(function _callee2$(_context2) {
      while (1) {
        switch (_context2.prev = _context2.next) {
          case 0:
            path = _ref4.path, reg = _ref4.reg, str = _ref4.str, _ref4$options = _ref4.options, options = _ref4$options === void 0 ? {
              encoding: 'utf8'
            } : _ref4$options;
            // home文件夹~, node不认, 要调整为os.homedir
            rpath = realpath(path); // 读取文件

            _context2.next = 4;
            return _promises["default"].readFile(rpath, options);

          case 4:
            fstr = _context2.sent;

            if (!reg.test(fstr)) {
              _context2.next = 7;
              break;
            }

            return _context2.abrupt("return", false);

          case 7:
            _context2.next = 9;
            return _promises["default"].appendFile(rpath, str != null ? str : reg.source, options);

          case 9:
            return _context2.abrupt("return", true);

          case 10:
          case "end":
            return _context2.stop();
        }
      }
    }, _callee2);
  }));

  return function uniqueadd(_x2) {
    return _ref5.apply(this, arguments);
  };
}(); // 判断是文件/目录


exports.uniqueadd = uniqueadd;

var filetype = /*#__PURE__*/function () {
  var _ref6 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee3(path) {
    var stat;
    return _regenerator["default"].wrap(function _callee3$(_context3) {
      while (1) {
        switch (_context3.prev = _context3.next) {
          case 0:
            _context3.next = 2;
            return _promises["default"].lstat(realpath(path));

          case 2:
            stat = _context3.sent;
            return _context3.abrupt("return", {
              isfile: stat.isFile(),
              isdir: stat.isDirectory()
            });

          case 4:
          case "end":
            return _context3.stop();
        }
      }
    }, _callee3);
  }));

  return function filetype(_x3) {
    return _ref6.apply(this, arguments);
  };
}(); // 判断文件/目录是否存在


exports.filetype = filetype;

var exist = function exist(path) {
  return _fs["default"].existsSync(realpath(path));
}; // 建一个目录


exports.exist = exist;

var mkdir = /*#__PURE__*/function () {
  var _ref7 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee4(path) {
    return _regenerator["default"].wrap(function _callee4$(_context4) {
      while (1) {
        switch (_context4.prev = _context4.next) {
          case 0:
            _context4.next = 2;
            return _promises["default"].mkdir(realpath(path), {
              recursive: true
            });

          case 2:
            return _context4.abrupt("return", _context4.sent);

          case 3:
          case "end":
            return _context4.stop();
        }
      }
    }, _callee4);
  }));

  return function mkdir(_x4) {
    return _ref7.apply(this, arguments);
  };
}(); // 如果不存在就建一个目录


exports.mkdir = mkdir;

var newdir = /*#__PURE__*/function () {
  var _ref8 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee5(path) {
    return _regenerator["default"].wrap(function _callee5$(_context5) {
      while (1) {
        switch (_context5.prev = _context5.next) {
          case 0:
            if (!exist(path)) {
              _context5.next = 2;
              break;
            }

            return _context5.abrupt("return", false);

          case 2:
            _context5.next = 4;
            return _promises["default"].mkdir(realpath(path));

          case 4:
            return _context5.abrupt("return", _context5.sent);

          case 5:
          case "end":
            return _context5.stop();
        }
      }
    }, _callee5);
  }));

  return function newdir(_x5) {
    return _ref8.apply(this, arguments);
  };
}(); // 读取目录


exports.newdir = newdir;

var readir = /*#__PURE__*/function () {
  var _ref9 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee6(path) {
    return _regenerator["default"].wrap(function _callee6$(_context6) {
      while (1) {
        switch (_context6.prev = _context6.next) {
          case 0:
            _context6.next = 2;
            return _promises["default"].readdir(realpath(path));

          case 2:
            return _context6.abrupt("return", _context6.sent);

          case 3:
          case "end":
            return _context6.stop();
        }
      }
    }, _callee6);
  }));

  return function readir(_x6) {
    return _ref9.apply(this, arguments);
  };
}(); // 读取文件


exports.readir = readir;

var readfile = /*#__PURE__*/function () {
  var _ref10 = (0, _asyncToGenerator2["default"])( /*#__PURE__*/_regenerator["default"].mark(function _callee7(path) {
    var fstr;
    return _regenerator["default"].wrap(function _callee7$(_context7) {
      while (1) {
        switch (_context7.prev = _context7.next) {
          case 0:
            _context7.next = 2;
            return _promises["default"].readFile(realpath(path), {
              encoding: 'utf8'
            });

          case 2:
            return _context7.abrupt("return", fstr = _context7.sent);

          case 3:
          case "end":
            return _context7.stop();
        }
      }
    }, _callee7);
  }));

  return function readfile(_x7) {
    return _ref10.apply(this, arguments);
  };
}(); // todo 读取目录中所有问题, 深度遍历


exports.readfile = readfile;

var allfile = function allfile(_ref11) {
  var path = _ref11.path,
      _ref11$reg = _ref11.reg,
      reg = _ref11$reg === void 0 ? false : _ref11$reg,
      _ref11$options = _ref11.options,
      options = _ref11$options === void 0 ? {
    encoding: 'utf8'
  } : _ref11$options;
  // 如果没传reg, 那么就是要所有文件
  return readdir(path, options);
};

exports.allfile = allfile;
var _default = {
  savefile: savefile,
  cover: cover,
  uniqueadd: uniqueadd,
  filetype: filetype,
  exist: exist,
  mkdir: mkdir,
  readir: readir,
  readfile: readfile,
  allfile: allfile
};
exports.default = _default;

/***/ }),

/***/ "./src/mlib/src/index.cs":
/*!*******************************!*
  !*** ./src/mlib/src/index.cs ***!
  \*******************************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "@babel/runtime/helpers/interopRequireDefault");

Object.defineProperty(exports, "__esModule", ({
  value: true
}));
Object.defineProperty(exports, "color", ({
  enumerable: true,
  get: function get() {
    return _color["default"];
  }
}));
Object.defineProperty(exports, "random", ({
  enumerable: true,
  get: function get() {
    return _random["default"];
  }
}));
Object.defineProperty(exports, "time", ({
  enumerable: true,
  get: function get() {
    return _time["default"];
  }
}));
Object.defineProperty(exports, "file", ({
  enumerable: true,
  get: function get() {
    return _file["default"];
  }
}));

var _color = _interopRequireDefault(__webpack_require__(/*! ./color.cs */ "./src/mlib/src/color.cs"));

var _random = _interopRequireDefault(__webpack_require__(/*! ./random.cs */ "./src/mlib/src/random.cs"));

var _time = _interopRequireDefault(__webpack_require__(/*! ./time.cs */ "./src/mlib/src/time.cs"));

var _file = _interopRequireDefault(__webpack_require__(/*! ./file.cs */ "./src/mlib/src/file.cs"));

/***/ }),

/***/ "./src/mlib/src/random.cs":
/*!********************************!*
  !*** ./src/mlib/src/random.cs ***!
  \********************************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";


Object.defineProperty(exports, "__esModule", ({
  value: true
}));
exports.default = exports.randomlight = exports.randomcolor = exports.randomember = exports.randomint = void 0;

var _color = __webpack_require__(/*! ./color.cs */ "./src/mlib/src/color.cs");

/*
* 拿到整数版本的随机数
* 因为random不包含1, 因此, 这个随机数也不包含max
* 因此传入数组的长度正好合适作为数组的索引
 */
var randomint = function randomint(max) {
  return Math.random() * max >> 0;
};
/*
数组中取一个随机element
*/


exports.randomint = randomint;

var randomember = function randomember(p) {
  return p[randomint(p.length)];
}; // 随机一个颜色


exports.randomember = randomember;

var randomcolor = function randomcolor() {
  return "rgb(" + randomint(256) + "," + randomint(256) + "," + randomint(256) + ")";
}; // 随机一个明亮颜色


exports.randomcolor = randomcolor;

var randomlight = function randomlight() {
  return (0, _color.lstr)({
    str: randomcolor(0),
    isback: false
  });
};

exports.randomlight = randomlight;
var _default = {
  randomint: randomint,
  randomember: randomember,
  randomcolor: randomcolor,
  randomlight: randomlight
};
exports.default = _default;

/***/ }),

/***/ "./src/mlib/src/time.cs":
/*!******************************!*
  !*** ./src/mlib/src/time.cs ***!
  \******************************/
/***/ ((__unused_webpack_module, exports) => {

"use strict";


Object.defineProperty(exports, "__esModule", ({
  value: true
}));
exports.default = exports.gethhmmss = exports.gethhmm = exports.sleep = void 0;

var sleep = function sleep(ms) {
  return new Promise(function (resolve) {
    return setTimeout(resolve, ms);
  });
};

exports.sleep = sleep;

var gethhmm = function gethhmm() {
  var timestamp = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : Date.now();
  var t;
  t = new Date(+timestamp);
  return t.getHours() + ':' + t.getMinutes();
};

exports.gethhmm = gethhmm;

var gethhmmss = function gethhmmss() {
  var timestamp = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : Date.now();
  var t;
  t = new Date(+timestamp);
  return t.getHours() + ':' + t.getMinutes() + ':' + t.getSeconds();
};

exports.gethhmmss = gethhmmss;
var _default = {
  sleep: sleep,
  gethhmm: gethhmm,
  gethhmmss: gethhmmss
};
exports.default = _default;

/***/ }),

/***/ "./src/mlib/src/colorname.cson":
/*!*************************************!*
  !*** ./src/mlib/src/colorname.cson ***!
  \*************************************/
/***/ ((module) => {

module.exports = {"black":"#000","silver":"#c0c0c0","gray":"#808080","white":"#fff","maroon":"#800000","red":"#ff0000","purple":"#800080","fuchsia":"#ff00ff","green":"#008000","lime":"#00ff00","olive":"#808000","yellow":"#ffff00","navy":"#000080","blue":"#0000ff","teal":"#008080","aqua":"#00ffff","orange":"#ffa500","aliceblue":"#f0f8ff","antiquewhite":"#faebd7","aquamarine":"#7fffd4","azure":"#f0ffff","beige":"#f5f5dc","bisque":"#ffe4c4","blanchedalmond":"#ffebcd","blueviolet":"#8a2be2","brown":"#a52a2a","burlywood":"#deb887","cadetblue":"#5f9ea0","chartreuse":"#7fff00","chocolate":"#d2691e","coral":"#ff7f50","cornflowerblue":"#6495ed","cornsilk":"#fff8dc","crimson":"#dc143c","cyan":"#00ffff","darkblue":"#00008b","darkcyan":"#008b8b","darkgoldenrod":"#b8860b","darkgray":"#a9a9a9","darkgreen":"#006400","darkgrey":"#a9a9a9","darkkhaki":"#bdb76b","darkmagenta":"#8b008b","darkolivegreen":"#556b2f","darkorange":"#ff8c00","darkorchid":"#9932cc","darkred":"#8b0000","darksalmon":"#e9967a","darkseagreen":"#8fbc8f","darkslateblue":"#483d8b","darkslategray":"#2f4f4f","darkslategrey":"#2f4f4f","darkturquoise":"#00ced1","darkviolet":"#9400d3","deeppink":"#ff1493","deepskyblue":"#00bfff","dimgray":"#696969","dimgrey":"#696969","dodgerblue":"#1e90ff","firebrick":"#b22222","floralwhite":"#fffaf0","forestgreen":"#228b22","gainsboro":"#dcdcdc","ghostwhite":"#f8f8ff","gold":"#ffd700","goldenrod":"#daa520","greenyellow":"#adff2f","grey":"#808080","honeydew":"#f0fff0","hotpink":"#ff69b4","indianred":"#cd5c5c","indigo":"#4b0082","ivory":"#fffff0","khaki":"#f0e68c","lavender":"#e6e6fa","lavenderblush":"#fff0f5","lawngreen":"#7cfc00","lemonchiffon":"#fffacd","lightblue":"#add8e6","lightcoral":"#f08080","lightcyan":"#e0ffff","lightgoldenrodyellow":"#fafad2","lightgray":"#d3d3d3","lightgreen":"#90ee90","lightgrey":"#d3d3d3","lightpink":"#ffb6c1","lightsalmon":"#ffa07a","lightseagreen":"#20b2aa","lightskyblue":"#87cefa","lightslategray":"#778899","lightslategrey":"#778899","lightsteelblue":"#b0c4de","lightyellow":"#ffffe0","limegreen":"#32cd32","linen":"#faf0e6","magenta":"#ff00ff","mediumaquamarine":"#66cdaa","mediumblue":"#0000cd","mediumorchid":"#ba55d3","mediumpurple":"#9370db","mediumseagreen":"#3cb371","mediumslateblue":"#7b68ee","mediumspringgreen":"#00fa9a","mediumturquoise":"#48d1cc","mediumvioletred":"#c71585","midnightblue":"#191970","mintcream":"#f5fffa","mistyrose":"#ffe4e1","moccasin":"#ffe4b5","navajowhite":"#ffdead","oldlace":"#fdf5e6","olivedrab":"#6b8e23","orangered":"#ff4500","orchid":"#da70d6","palegoldenrod":"#eee8aa","palegreen":"#98fb98","paleturquoise":"#afeeee","palevioletred":"#db7093","papayawhip":"#ffefd5","peachpuff":"#ffdab9","peru":"#cd853f","pink":"#ffc0cb","plum":"#dda0dd","powderblue":"#b0e0e6","rosybrown":"#bc8f8f","royalblue":"#4169e1","saddlebrown":"#8b4513","salmon":"#fa8072","sandybrown":"#f4a460","seagreen":"#2e8b57","seashell":"#fff5ee","sienna":"#a0522d","skyblue":"#87ceeb","slateblue":"#6a5acd","slategray":"#708090","slategrey":"#708090","snow":"#fffafa","springgreen":"#00ff7f","steelblue":"#4682b4","tan":"#d2b48c","thistle":"#d8bfd8","tomato":"#ff6347","transparent":"#0000","turquoise":"#40e0d0","violet":"#ee82ee","wheat":"#f5deb3","whitesmoke":"#f5f5f5","yellowgreen":"#9acd32","rebeccapurple":"#663399"}

/***/ }),

/***/ "@babel/runtime/helpers/asyncToGenerator":
/*!**********************************************************!*
  !*** external "@babel/runtime/helpers/asyncToGenerator" ***!
  \**********************************************************/
/***/ ((module) => {

"use strict";
module.exports = require("@babel/runtime/helpers/asyncToGenerator");;

/***/ }),

/***/ "@babel/runtime/helpers/interopRequireDefault":
/*!***************************************************************!*
  !*** external "@babel/runtime/helpers/interopRequireDefault" ***!
  \***************************************************************/
/***/ ((module) => {

"use strict";
module.exports = require("@babel/runtime/helpers/interopRequireDefault");;

/***/ }),

/***/ "@babel/runtime/regenerator":
/*!*********************************************!*
  !*** external "@babel/runtime/regenerator" ***!
  \*********************************************/
/***/ ((module) => {

"use strict";
module.exports = require("@babel/runtime/regenerator");;

/***/ }),

/***/ "chalk":
/*!************************!*
  !*** external "chalk" ***!
  \************************/
/***/ ((module) => {

"use strict";
module.exports = require("chalk");;

/***/ }),

/***/ "fs":
/*!*********************!*
  !*** external "fs" ***!
  \*********************/
/***/ ((module) => {

"use strict";
module.exports = require("fs");;

/***/ }),

/***/ "fs/promises":
/*!******************************!*
  !*** external "fs/promises" ***!
  \******************************/
/***/ ((module) => {

"use strict";
module.exports = require("fs/promises");;

/***/ }),

/***/ "os":
/*!*********************!*
  !*** external "os" ***!
  \*********************/
/***/ ((module) => {

"use strict";
module.exports = require("os");;

/***/ })

};
;

// load runtime
var __webpack_require__ = require("../runtime/runtime.js");
__webpack_require__.C(exports);
return __webpack_require__.X([], "./src/lornpughelp.cs");
})();
});
//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly9tbGliL3dlYnBhY2svdW5pdmVyc2FsTW9kdWxlRGVmaW5pdGlvbiIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL2xvcm5wdWdoZWxwLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvY29sb3IuY3MiLCJ3ZWJwYWNrOi8vbWxpYi8uL3NyYy9tbGliL3NyYy9maWxlLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvaW5kZXguY3MiLCJ3ZWJwYWNrOi8vbWxpYi8uL3NyYy9tbGliL3NyYy9yYW5kb20uY3MiLCJ3ZWJwYWNrOi8vbWxpYi8uL3NyYy9tbGliL3NyYy90aW1lLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvY29sb3JuYW1lLmNzb24iLCJ3ZWJwYWNrOi8vbWxpYi9leHRlcm5hbCBcIkBiYWJlbC9ydW50aW1lL2hlbHBlcnMvYXN5bmNUb0dlbmVyYXRvclwiIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJAYmFiZWwvcnVudGltZS9oZWxwZXJzL2ludGVyb3BSZXF1aXJlRGVmYXVsdFwiIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJAYmFiZWwvcnVudGltZS9yZWdlbmVyYXRvclwiIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJjaGFsa1wiIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJmc1wiIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJmcy9wcm9taXNlc1wiIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJvc1wiIl0sIm5hbWVzIjpbXSwibWFwcGluZ3MiOiI7QUFBQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQSxDQUFDO0FBQ0QsTzs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7QUNWQTs7QUFFQTs7QUFGQTs7OztBQU1BLElBQU8sUUFBTyxTQUFQLEtBQU87U0FDYixhO0FBUEQsQ0FNQSxDOzs7OztBQUlBLElBQU87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QUFGbkIsQ0FBbEI7OztlQU9lO0FBQ2QsT0FEYyxFQUNkLEtBRGM7QUFFZCxTQUZjLEVBRWQsT0FGYztBQUdkLFdBSGMsRUFHZDtBQUhjLEM7Ozs7Ozs7Ozs7O0FDaEJmLGtCQUFrQiw4ckc7Ozs7Ozs7Ozs7O0FDQWxCLHFFOzs7Ozs7Ozs7OztBQ0FBLDBFOzs7Ozs7Ozs7OztBQ0FBLHdEOzs7Ozs7Ozs7OztBQ0FBLG1DOzs7Ozs7Ozs7OztBQ0FBLGdDOzs7Ozs7Ozs7OztBQ0FBLHlDOzs7Ozs7Ozs7OztBQ0FBLGdDIiwiZmlsZSI6Imxvcm5wdWdoZWxwL2xvcm5wdWdoZWxwLmpzIiwic291cmNlc0NvbnRlbnQiOlsiKGZ1bmN0aW9uIHdlYnBhY2tVbml2ZXJzYWxNb2R1bGVEZWZpbml0aW9uKHJvb3QsIGZhY3RvcnkpIHtcblx0aWYodHlwZW9mIGV4cG9ydHMgPT09ICdvYmplY3QnICYmIHR5cGVvZiBtb2R1bGUgPT09ICdvYmplY3QnKVxuXHRcdG1vZHVsZS5leHBvcnRzID0gZmFjdG9yeSgpO1xuXHRlbHNlIGlmKHR5cGVvZiBkZWZpbmUgPT09ICdmdW5jdGlvbicgJiYgZGVmaW5lLmFtZClcblx0XHRkZWZpbmUoW10sIGZhY3RvcnkpO1xuXHRlbHNlIGlmKHR5cGVvZiBleHBvcnRzID09PSAnb2JqZWN0Jylcblx0XHRleHBvcnRzW1wibWxpYlwiXSA9IGZhY3RvcnkoKTtcblx0ZWxzZVxuXHRcdHJvb3RbXCJtbGliXCJdID0gZmFjdG9yeSgpO1xufSkoZ2xvYmFsLCBmdW5jdGlvbigpIHtcbnJldHVybiAiLCJpbXBvcnQgY2hhbGsgZnJvbSAnY2hhbGsnXG4jY2hhbGsgPSByZXF1aXJlKCdjaGFsaycpXG5pbXBvcnQge2ZpbGV9IGZyb20gJ21saWInXG5cbiMgLS0tLS0tLS0tLS0tLS0tLS1wdXJlIGZ1bmN0aW9uLS0tLS0tLS0tLS0tLS0tLS1cbiMg5Yik5pat5piv5ZCm5pyJ5ZG95Luk5Y+C5pWwKC0t5byA5aeL55qE5ZG95Luk5Y+C5pWwKSwg5rKh5pyJLS3nmoTpg73mmK/mlofku7ZcbmV4cG9ydCBpc2NtZD0gKHgpLT5cblx0L14tLS8udGVzdCB4XG5cbiMg5aSE55CGbHDlkb3ku6RcbmV4cG9ydCBkb2xwID0gLT5cblx0c3RyPScnJ1xuXHQjIOiIlOeLl+mhueebrlxuXHRhbGlhcyBscD1cImxwZ1wiXG5cdCcnJ1xuXHRyZWc9L2FsaWFzXFxzKmxwPVwibHBnXCIvXG5cdG9rPWF3YWl0IGZpbGUudW5pcXVlYWRkIHtcblx0XHRyZWdcblx0XHRzdHJcblx0XHRwYXRoOid+Ly56c2hyYydcblx0XHQjcGF0aDogJ1JFQURNRS5tZCdcblx0fVxuXHRpZiBva1xuXHRcdGNvbnNvbGUubG9nICggJ+WRveS7pOiuvue9ruS4umxwJyksIChjaGFsay5ncmVlbkJyaWdodCAn5oiQ5YqfOiAnKSwn6ZyA6KaB5Yi35paw6YWN572u5omN5Lya55Sf5pWILCDmjqjojZDkvb/nlKjlv6vmjbfph43lkK/lkb3ku6Q6J1xuXHRcdGNvbnNvbGUubG9nIChjaGFsay5ncmVlbkJyaWdodCAnZXhlYyB6c2gnKSwoY2hhbGsueWVsbG93ICcjIOmHjeWQr+W9k+WJjXRlcm1pbmFsJylcblxuXHRcdGNvbnNvbGUubG9nICAn5aaC5p6c5LiN6IO96YeN5ZCvdGVybWluYWw6ICdcblx0XHRjb25zb2xlLiBsb2cgKGNoYWxrLmdyZWVuQnJpZ2h0ICdzb3VyY2Ugfi8uenNocmMnKSwgKGNoYWxrLnllbGxvdyAnIyDlr7zlhaXphY3nva7mlofku7bmlrnms5XkuIAnKVxuXHRcdGNvbnNvbGUuIGxvZyAoY2hhbGsuZ3JlZW5CcmlnaHQgJy4gfi8uenNocmMnKSwgKGNoYWxrLnllbGxvdyAnIyDlr7zlhaXphY3nva7mlofku7bmlrnms5XkuownKVxuXHRlbHNlXG5cdFx0Y29uc29sZS5sb2cgKGNoYWxrLnJlZEJyaWdodCAn5rKh5pyJJyksICggJ+i/m+ihjOiuvue9riwg5Y+v6IO95piv6YWN572u5paH5Lu25bey57uP5a2Y5ZyoJyksIChjaGFsay55ZWxsb3cgJ+ivt+aJi+W3peajgOafpeaWh+S7tjogJyksJ34vLnpzaHJjJ1xuXG4jIOWkhOeQhuS4gOS4qmNz5oiW6ICFY29mZmVl55qE5Yid5aeL5YyWXG5kaXIxZmlsZSA9IChwYXRoKS0+XG5cdHA9IHBhdGgrJy5scCdcblx0IyDliKTmlq3lr7nlupTnm67lvZXmmK/lkKblrZjlnKguXG5cdHJldHVybiBpZiAoZmlsZS5leGlzdCBwKVxuXHQjIOaWsOW7uuebruW9lVxuXHRhd2FpdCBmaWxlLm1rZGlyIHBcblx0IyB0b2Rv6K+75Y+W5paH5Lu2LCDliIbmnpDlh73mlbAsIOW7uueri+WvueW6lOeahOa1i+ivleaWh+S7tlxuXHQjIOWIneWni+WMlua1i+ivleaWh+S7tlxuXHRhd2FpdCBmaWxlLmNvdmVyIHBhdGg6cCsnL2FhYS4waS5jc29uJyxzdHI6Jydcblx0YXdhaXQgZmlsZS5jb3ZlciBwYXRoOnArJy9hYWEuMG8uY3Nvbicsc3RyOicnXG5cdGF3YWl0IGZpbGUuY292ZXIgcGF0aDpwKycvY3RvLmNzb24nLHN0cjonJ1xuXG4jIOWkhOeQhuWIneWni+WMlua1i+ivlWRpclxuZXhwb3J0IGRvZGlyID0gKHBhdGgpLT5cblx0c3RhdD0gYXdhaXQgZmlsZS5maWxldHlwZSBwYXRoXG5cdGlmIHN0YXQuaXNmaWxlICMg5paH5Lu25aSE55CGXG5cdFx0IyDlpITnkIZjb2ZmZWXlkoxjc1xuXHRcdHJldHVybiB1bmxlc3MgL1xcLmNvZmZlZSR8XFwuY3MkLy50ZXN0IHBhdGhcblx0XHRkaXIxZmlsZSBwYXRoXG5cdGlmIHN0YXQuaXNkaXIgIyDnm67lvZXlpITnkIZcblx0XHQjIOmAkuW9kuWkhOeQhuebruW9lSwg6KaB6YG/5byA6Ieq5Yqo55Sf5oiQ55qE55uu5b2VKOWQjeensOmHjOmdouW4pueCueeahClcblx0XHRyZXR1cm4gaWYgL1xcLi8udGVzdCBwYXRoXG5cdFx0IyDor7vlj5bnm67lvZUsIOmAkOmhueWkhOeQhlxuXHRcdGRvZGlyIHggZm9yIHggaW4gYXdhaXQgZmlsZS5yZWFkaXIgcGF0aFxuXHRyZXR1cm4gIyDpmaTkuoZmaWxl5ZKMZGly5Y+v6IO96L+Y5pyJ5YW25LuW5oOF5Ya1LCDmr5TlpoLnrKblj7fpk77mjqVcblxuZXhwb3J0IGRlZmF1bHQge1xuXHRpc2NtZFxuXHRkb2xwXG5cdGRvZGlyXG59XG5cbmRvZGlyICcuJ1xuIiwiIyMjXG4gKiDlhbPkuo7popzoibLnmoTlupMsIOi/meS4quW6k+S8muiiq+mHjeWGmSwg5Zug5Li6aHNs55qE5Lqu5bqm5Yik5pat5Y+v5Lul6K6k5Li65piv6ZSZ55qELiDmiYDku6Xlj6/ku6Xkv53nlZloc2znm7jlhbPnmoTlh73mlbAsIOS9huaYrywg5a6e6ZmF5bqU55So5Ye95pWw6ZyA6KaB6YeN5YaZLlxuICpcbiMjI1xuaW1wb3J0IGNvbG9ybmFtZSBmcm9tICcuL2NvbG9ybmFtZS5jc29uJ1xuXG5kYXJrY29sb3I9ICdyZ2JhKDQ0LDQ0LDQ0LDEpJyAjZGFya+eahOWfuuehgOminOiJslxubGlnaHRjb2xvcj0gJ3JnYmEoMjAwLDIwMCwyMDAsMSknXG5cblxuXG5cblxuIyMjKlxuICogLS0tLS0tLS0tLS0tLS0tLS0tLS0t5Y2V54us5aSE55CG5YmN5pmv5oiW6ICF6IOM5pmvLCDphY3lkIhkYXJrY3Nz55qE5oCd6LevLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tXG4gKiDkuq7luqbliKTmlq3pgLvovpEgZz4xMDAsIHI+MjAwLCDmiJbogIVnK3I+MzAwXG4gKiDkuq7luqblpITnkIbpgLvovpEgdD1tYXgoMTAwLWcsIDIwMC1yKSwgdCtyLCB0K2csIHQrYlxuICpcbiAqIOaal+W6puWIpOaWremAu+i+kSBnPDYwIHI8MTAwIGI8MTUwXG4gKiDmmpfluqblpITnkIbpgLvovpEgdD1tYXgoZy02MCwgci0xMDAsIGItMTUwKSwgci10LCBnLXQsIGItdFxuICpcbiAjIyNcblxuXHQjIyMqXG5cdCAqIHJnYm9rXG5cdCAqIOWIpOaWreminOiJsuaYr+WQpumcgOimgeWkhOeQhlxuXHQgKiAgKiDkuq7luqbliKTmlq3pgLvovpEgZz4xMDAsIHI+MjAwLCDmiJbogIVnK3I+MzAwXG5cdCAqICAqIOaal+W6puWIpOaWremAu+i+kSBnPDYwIHI8MTAwIGI8MTUwXG5cdCAqIOWPguaVsFxuXHQgKiBcdHJnYjp7cixnLGJ9XG5cdCAqICBpc2JhY2s6IOaYr+WQpuS4uuiDjOaZr+WbvueJh1xuXHQgKiDov5Tlm57lgLxcblx0ICogXHRcdHRydWU6IOS4jemcgOimgeWkhOeQhlxuXHQgKiBcdFx0ZmFsc2U6IOmcgOimgeWkhOeQhlxuXHQgIyMjXG5leHBvcnQgcmdib2s9KHtyZ2IsaXNiYWNrPXRydWV9KS0+XG5cdHtyLGcsYn09cmdiXG5cdGlmIGlzYmFjayB0aGVuIFx0Zzw2MCAmJiByPDEwMCAmJiBiPDE1MCBlbHNlIHIrZz4zMDBcblx0XHRcdCPmmpfluqbliKTmlq1cblx0XHRcdCPlhYnmmI7mqKHlvI8sIOWmguaenOS6ruW6pjwwLjfliJnpnIDopoHlpITnkIYsIOWQpuWImeWwseS4jemcgOimgeWkhOeQhlxuXG5cbiMjIypcbiAqIOS7jnN0cuWIsOiwg+S6rueahOminOiJsnN0clxuICogIOi/memHjOWkhOeQhuS4gOS4qnN0eWxlIHN0cmluZ+eahOS6ruW6plxuICog57qv5Ye95pWwXG4gKiBzdHI6IOS4gOS4quminOiJsuWtl+espuS4slxuICogaXNiYWNrOiDmmK/lkKbkuLrog4zmma9cbiAqIOW6n+W8gyByZXR1cm4gZmFsc2Ug5bm25LiN5ZCI55CGXG4jIyNcbmV4cG9ydCBsc3RyPSh7c3RyLGlzYmFjaz10cnVlfSkgLT5cblx0cmV0dXJuIGZhbHNlIGlmKCFzdHIpXG5cdFxuXHQj5aaC5p6c5pivaW5pdGlhbCwg6YKj5LmI55u05o6l55So5oiR55qE5YC8LlxuXHRyZXR1cm4gKGlmIGlzYmFjayB0aGVuIGRhcmtjb2xvciBlbHNlIGxpZ2h0Y29sb3IpIGlmIHN0cj09XCJpbml0aWFsXCJcblx0XG5cdCNmaXhtZSDmmoLml7blnKjov5nph4zlv73nlaVjc3MgdmFyICAjdG9kb1xuXHRyZXR1cm4gKGlmIGlzYmFjayB0aGVuIGRhcmtjb2xvciBlbHNlIGxpZ2h0Y29sb3IpaWYgL3ZhclxcKC0tLy50ZXN0KHN0cilcblxuXHRsPWxyZ2Ige3JnYjogc3RyMnJnYihzdHIpLGlzYmFja31cblx0cmdiMnN0cihsKVxuXG4jIyMqXG5cdCAqIGxpZ2h0cmdi5qC45b+D5Lqu5bqm5aSE55CG5Ye95pWwXG5cdCAqICAqIOS6ruW6puWkhOeQhumAu+i+kSB0PW1heCgxMDAtZywgMjAwLXIpLCB0K3IsIHQrZywgdCtiXG5cdCAqICAqIOaal+W6puWkhOeQhumAu+i+kSB0PW1heChnLTYwLCByLTEwMCwgYi0xNTApLCByLXQsIGctdCwgYi10XG5cdCAqIOW6n+W8g3JldHVybiBmYWxzZeW5tuS4jeWQiOeQhlxuIyMjXG5leHBvcnQgbHJnYj0oe3JnYixpc2JhY2s9dHJ1ZX0pIC0+XG5cdHJldHVybiByZ2IgaWYgcmdib2sge3JnYixpc2JhY2t9XG5cdHtyLGcsYn09cmdiXG5cdGlmIChpc2JhY2spICAj5aSE55CG5pqX6buR6IOM5pmvXG5cdFx0dD0gTWF0aC5tYXgoZy02MCxyLTEwMCwgYi0xNTApXG5cdFx0ci09dFxuXHRcdGctPXRcblx0XHRiLT10XG5cdGVsc2UgICPlpITnkIbmmI7kuq7liY3mma9cblx0XHR0PSBNYXRoLm1heCgxMDAtZywyMDAtcilcblx0XHRyKz10XG5cdFx0Zys9dFxuXHRcdGIrPXRcblx0e3IsZyxifVxuXG4jIyMqXG4gKiDku45zdHLliLDosIPkuq7nmoTpopzoibJzdHJcbiAqICDov5nph4zlpITnkIbkuIDkuKpzdHlsZSBzdHJpbmfnmoTkuq7luqZcbiAqIOe6r+WHveaVsFxuICogc3RyOiDkuIDkuKrpopzoibLlrZfnrKbkuLJcbiAqIGlzYmFjazog5piv5ZCm5Li66IOM5pmvXG4gKiDlup/lvIMgcmV0dXJuIGZhbHNlIOW5tuS4jeWQiOeQhlxuIyMjXG5saWdodHN0cj0oe3N0cixpc2JhY2s9dHJ1ZX0pIC0+XG5cdHJldHVybiBmYWxzZSBpZighc3RyKVxuXHRcblx0I+WmguaenOaYr2luaXRpYWwsIOmCo+S5iOebtOaOpeeUqOaIkeeahOWAvC5cblx0cmV0dXJuIChpZiBpc2JhY2sgdGhlbiBkYXJrY29sb3IgZWxzZSBsaWdodGNvbG9yKSBpZiBzdHI9PVwiaW5pdGlhbFwiXG5cdFxuXHQjZml4bWUg5pqC5pe25Zyo6L+Z6YeM5b+955WlY3NzIHZhciAgI3RvZG9cblx0cmV0dXJuIChpZiBpc2JhY2sgdGhlbiBkYXJrY29sb3IgZWxzZSBsaWdodGNvbG9yKWlmIC92YXJcXCgtLS8udGVzdChzdHIpXG5cdGw9bGlnaHRyZ2Ige3JnYjogc3RyMnJnYihzdHIpLGlzYmFja31cblx0aWYgbCB0aGVuIHJnYjJzdHIobCkgZWxzZSBmYWxzZVxuXG5cblxuIyMjKlxuXHQgKiBsaWdodHJnYuaguOW/g+S6ruW6puWkhOeQhuWHveaVsFxuXHQgKiAgKiDkuq7luqblpITnkIbpgLvovpEgdD1tYXgoMTAwLWcsIDIwMC1yKSwgdCtyLCB0K2csIHQrYlxuXHQgKiAgKiDmmpfluqblpITnkIbpgLvovpEgdD1tYXgoZy02MCwgci0xMDAsIGItMTUwKSwgci10LCBnLXQsIGItdFxuXHQgKiDlup/lvINyZXR1cm4gZmFsc2XlubbkuI3lkIjnkIZcbiMjI1xubGlnaHRyZ2I9KHtyZ2IsaXNiYWNrPXRydWV9KSAtPlxuXHRyZXR1cm4gZmFsc2UgaWYocmdib2soe3JnYixpc2JhY2t9KSlcblx0e3IsZyxifT1yZ2Jcblx0aWYgKGlzYmFjaykgICPlpITnkIbmmpfpu5Hog4zmma9cblx0XHR0PSBNYXRoLm1heChnLTYwLHItMTAwLCBiLTE1MClcblx0XHRyLT10XG5cdFx0Zy09dFxuXHRcdGItPXRcblx0ZWxzZSAgI+WkhOeQhuaYjuS6ruWJjeaZr1xuXHRcdHQ9IE1hdGgubWF4KDEwMC1nLDIwMC1yKVxuXHRcdHIrPXRcblx0XHRnKz10XG5cdFx0Yis9dFxuXHR7cixnLGJ9XG5cblxuXG5cblxuIyMjKlxuICogLS0tLS0tLS0tLS0tLS3lr7nmr5TluqbpgLvovpEsIOmFjeWQiGRhcmtzdHlsZS0tLS0tLS0tLS0tLS0tLS0tXG4gKiDov5nph4znmoTorqHnrpfmnKrlv4XlkIjpgIIuXG4jIyNcblxuXG5cblxuXHRcdCMjIypcblx0XHQgKiDku45zdHLliLDkuq7luqZcblx0XHQgIyMjXG5leHBvcnQgZ2V0bGlnaHQ9IChzdHIpIC0+XG5cdGdldHJnYmxpZ2h0IHN0cjJyZ2Igc3RyXG5cdFx0XG5leHBvcnQgZ2V0ZGFyaz0oc3RyKSAtPlxuXHRnZXRyZ2JkYXJrIHN0cjJyZ2Igc3RyXG5cdFx0XG5cblxuXG4jIyMqXG5cdCog5ou/5Yiw6KeG6KeJ5Lqu5bqmXG5cdCog55So5p2l5Yik5pat6aKc6Imy5piv5ZCm5aSf5LquXG5cdCog5pyq5p2l55So6I2n5YWJ5L2T5Y+R5YWJ6IO96YeP5p2l6K6h566X5pWw5YC8LlxuIyMjXG5nZXRyZ2JsaWdodD0gKHtyLGcsYn0pLT5cblx0cj1yKjEuMlxuXHRnPWcqMS43XG5cdG1saWdodCh7cixnLGJ9KS5saWdodFxuXG4jIyMqXG5cdCog5ou/5Yiw6KeG6KeJ5pqX5bqmXG5cdCog55So5p2l5Yik5pat6aKc6Imy5piv5ZCm5aSf5pqXXG5cdCpcbiMjI1xuZ2V0cmdiZGFyaz0oe3IsZyxifSktPlxuXHRyPXIqMS4zXG5cdGc9ZyoxLjJcblx0bWxpZ2h0KHtyLGcsYn0pLmxpZ2h0XG5cbm1saWdodD0oe3IsZyxifSktPlxuXHRpZiByID4gZ1xuXHRcdGIxPXJcblx0XHRiMj1nXG5cdGVsc2Vcblx0XHRiMT1nXG5cdFx0YjI9clxuXHRcblx0aWYgYiA8IGIyXG5cdFx0YjM9YlxuXHRlbHNlIGlmIGIgPCBiMVxuXHRcdGIzPWIyXG5cdFx0YjI9YlxuXHRlbHNlXG5cdFx0YjM9YjJcblx0XHRiMj1iMVxuXHRcdGIxPWJcblx0XHRcdCMg6L+Z6YeM5b6I5LiN5Lil6LCoLCDkuq7luqbnmoTlv4PnkIbpmIjpmZDmtYvph4/miJborrjog73orqnov5nkuKrorqHnrpfmm7Tnsr7noa4uXG5cdGxpZ2h0OiAoYjErYjIvNikqMi8zLG1heDogW2IxLGIyLGIzXVxuXHRcblxuXG5cblxuXG5cblxuXG4jIyNcblx0KiDln7rnoYDlt6Xlhbflh73mlbAtLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tXG5cdCog5Yik5pat5Lik5Liqcmdic3Ry5a6e6ZmF55u4562JXG4jIyNcblxuZXhwb3J0IHNhbWVyZ2JzdHI9KHJnYnN0cjEscmdic3RyMiktPlxuXHR7cjogcjEsZzogZzEsYjogYjF9PXN0cjJyZ2Iocmdic3RyMSlcblx0e3I6IHIyLGc6IGcyLGI6IGIyfT1zdHIycmdiKHJnYnN0cjIpXG5cdHIxPT1yMiYmZzE9PWcyJiZiMT09YjJcblxuXG4jIyMqXG4qIDPnp43mg4XlhrVcbiogMS4gcmdi5oOF5Ya1OiDku45cInJnYigxNTQsIDE1NCwgMClcIui9rOWMluS4ujoge3I6MTU0LGc6MTU0LGI6MCx9XG4qIFwicmdiYSgwLCAwLCAwLCAwKVwiIOi/mOmcgOWFvOWuueatpOenjeaDheWGtVxuKiAyLiAxNui/m+WItuaDheWGtTogICNmYzIgICNmYzI5ICNmYzI5MjkyMyAjZmMyOTI5XG4qIDMuIOminOiJsuWQjTogd2hpdGUsIGJsYWNrLCBvcmdhbmdlXG4qIyMjXG5cbnN0cjJyZ2I9KHN0ciktPlxuXG5cdCNcInJnYigxNTQsIDE1NCwgMClcIlxuXHRyZWc9L3JnYmE/XFwoKFswLTldezEsM30pLCA/KFswLTldezEsM30pLCA/KFswLTldezEsM30pKCwgPyhbMC05LiVdKykpP1xcKS9pXG5cdGM9cmVnLmV4ZWMgc3RyXG5cblx0IzE26L+b5Yi26aKc6ImyXG5cdGM/PWhleDJyZ2Ioc3RyKVxuXG5cdCPlho3liKTmlq3lkI3lrZfpopzoibI6XG5cdGM/PSBjb2xvcm5hbWVbc3RyXSAmJiBoZXgycmdiKGhleClcblx0XG5cdHJldHVybiBmYWxzZSB1bmxlc3MgYz9cblxuXG5cdCPpgI/mmI7luqZcblx0YT0gaWYgY1s1XT8gdGhlbiBjWzVdIGVsc2UgMVxuXHR7cjogK2NbMV0sZzogK2NbMl0sYjogK2NbM10sYX0gI+i/memHjOeUqCvmmK/kuLrkuobmiorlrZfnrKbkuLLovazkuLrmlbDlrZcsIOS4jeeEtuS8muWPkeeUn+W+iOmakOiUveeahGJ1Zy5cblxuXG5cdCMjI1xuXHQgKiDku45yZ2LliLBzdHJcblx0ICojIyNcbnJnYjJzdHI9KHtyLGcsYixhPTF9KSAtPlxuXHRpZiBhPjAuOTkgdGhlbiAgXCJyZ2IoXCIrcitcIixcIitnK1wiLFwiK2IrXCIpXCIgZWxzZSBcInJnYmEoXCIrcitcIixcIitnK1wiLFwiK2IrXCIsXCIrYStcIilcIlxuXG5cblx0IyMjXG5cdCAqIOS7jjE26L+b5Yi25YiwcmdiXG5cdCAqICAjZmMyICAjZmMyOSAjZmMyOTI5MjMgI2ZjMjkyOSwgMyw0LDYsOOmDveacieWPr+iDvVxuXHQjIyNcblxuXG5oZXgycmdiPShzdHIpIC0+XG5cdHJlZz0vIyhbXiNdKikvaSAj5Yik5patIzYyZmE4Oei/meenjeexu+S8vOaWueW8j+eahOminOiJsuWAvFxuXHRjPXJlZy5leGVjKHN0cilcblx0cmV0dXJuIGZhbHNlIHVubGVzcyBjP1sxXT8gXHQjYzHkuI3lrZjlnKjpnIDopoFyZXR1cm5cblx0eD1jWzFdXG5cdHk9W11cblx0c3dpdGNoIHgubGVuZ3RoXG5cdFx0d2hlbiAzXG5cdFx0XHR5WzFdPXBhcnNlSW50KCcnK3hbMF0reFswXSwxNilcblx0XHRcdHlbMl09cGFyc2VJbnQoJycreFsxXSt4WzFdLDE2KVxuXHRcdFx0eVszXT1wYXJzZUludCgnJyt4WzJdK3hbMl0sMTYpXG5cdFx0d2hlbiA0XG5cdFx0XHQjeVsxXT1wYXJzZUludCh4WzBdLDE2KTsgIC8yMDIwMTAyOOS/ruato+mUmeivrywg6L+Z5Liq5Lya5oqKZmZm6L2s5YyW5Li6IHIxNWcxNWIxNSwg5YW25a6e5bqU6K+l5pivcjI1NWcyNTViMjU1XG5cdFx0XHR5WzFdPXBhcnNlSW50KCcnK3hbMF0reFswXSwxNilcblx0XHRcdHlbMl09cGFyc2VJbnQoJycreFsxXSt4WzFdLDE2KVxuXHRcdFx0eVszXT1wYXJzZUludCgnJyt4WzJdK3hbMl0sMTYpXG5cdFx0XHR5WzVdPXBhcnNlSW50KHhbM10sMTYpLzI1NVxuXHRcdHdoZW4gNlxuXHRcdFx0eVsxXT1wYXJzZUludCgnJyt4WzBdK3hbMV0sMTYpXG5cdFx0XHR5WzJdPXBhcnNlSW50KCcnK3hbMl0reFszXSwxNilcblx0XHRcdHlbM109cGFyc2VJbnQoJycreFs0XSt4WzVdLDE2KVxuXHRcdHdoZW4gOFxuXHRcdFx0eVsxXT1wYXJzZUludCgnJyt4WzBdK3hbMV0sMTYpXG5cdFx0XHR5WzJdPXBhcnNlSW50KCcnK3hbMl0reFszXSwxNilcblx0XHRcdHlbM109cGFyc2VJbnQoJycreFs0XSt4WzVdLDE2KVxuXHRcdFx0eVs1XT1wYXJzZUludCgnJyt4WzZdK3hbN10sMTYpLzI1NVxuXHRcdCPkuI3mu6HotrPkuIrpnaLlm5vkuKrmg4XlhrUsIOebtOaOpei/lOWbnmZhbHNlXG5cdFx0ZWxzZSByZXR1cm4gZmFsc2Vcblx0eVxuXG5cdCNjb25zb2xlLmxvZyhzdHIycmdiKCd3aGl0ZScpKTsgI3Rlc3RcblxuXG5cblxuXG5cbnRlc3QgPSAtPlxuXHRyZXN1bHQ9d2luZG93Lmx1b2Nsci5saWdodHN0ciBzdHI6J3doaXRlJywgaXNiYWNrOnRydWVcblx0Y29uc29sZS5sb2cge3Jlc3VsdH1cblxuXG5leHBvcnQgZGVmYXVsdCB7XG5cdCNsaWdodHN0clxuXHQjbGlnaHRyZ2Jcblx0bHN0clxuXHRscmdiXG5cdGdldGxpZ2h0XG5cdGdldGRhcmtcblx0c2FtZXJnYnN0clxuXG59XG4iLCIjaW1wb3J0IHtsb2d9IGZyb20gJy4vbG9nLmpzJ1xuXG5pbXBvcnQgZnMgZnJvbSAnZnMvcHJvbWlzZXMnXG5pbXBvcnQgZnRzIGZyb20gJ2ZzJ1xuXG5pbXBvcnQgb3MgZnJvbSAnb3MnXG5cbmhvbWUgPSBvcy5ob21lZGlyKClcbmxvZz1jb25zb2xlLmxvZ1xuXG5cbiMgdG9kbyDkv53lrZjmlofku7ZcbmV4cG9ydCBzYXZlZmlsZT0oe2pzb24sZmlsZW5hbWU9XCJ0ZW1wdGVzdC50eHRcIn0pLT5cblx0ZnMud3JpdGVGaWxlIGZpbGVuYW1lLEpTT04uc3RyaW5naWZ5KGpzb24pLChlcnIpLT5cblx0XHRsb2cgJ+aWh+S7tuWGmeWFpScsZmlsZW5hbWVcblx0XHRsb2cgZXJyIGlmIGVyclxuXG4jIOaLv+WIsOecn+ato+eahHBhdGhcbmV4cG9ydCByZWFscGF0aD0ocGF0aCktPlxuXHQjIGhvbWXmlofku7blpLl+LCBub2Rl5LiN6K6kLCDopoHosIPmlbTkuLpvcy5ob21lZGlyXG5cdGlmIC9efi8udGVzdCBwYXRoIHRoZW4gcGF0aC5yZXBsYWNlIC9efi8sIGhvbWUgIGVsc2UgcGF0aFxuXG5cbiMg5Yqg5paH5Lu25YaF5a65LCDopobnm5blvI9cbmV4cG9ydCBjb3Zlcj0oe3BhdGgsIHN0ciwgb3B0aW9ucz1lbmNvZGluZzondXRmOCd9KSAtPlxuXG5cdCMg6KaG55uW5byP5YaZ5YWlXG5cdGF3YWl0IGZzLndyaXRlRmlsZSAocmVhbHBhdGggcGF0aCksIHN0ciwgb3B0aW9uc1xuXHR0cnVlXG4jIOi/veWKoOaWh+S7tuWGheWuuSwg5L+d6K+B6L+95Yqg5YaF5a655LiN6YeN5aSNXG5leHBvcnQgdW5pcXVlYWRkPSh7cGF0aCwgcmVnLCBzdHIsIG9wdGlvbnM9ZW5jb2Rpbmc6J3V0ZjgnfSkgLT5cblxuXHQjIGhvbWXmlofku7blpLl+LCBub2Rl5LiN6K6kLCDopoHosIPmlbTkuLpvcy5ob21lZGlyXG5cdHJwYXRoID1yZWFscGF0aCBwYXRoXG5cblx0IyDor7vlj5bmlofku7Zcblx0ZnN0cj1hd2FpdCBmcy5yZWFkRmlsZSBycGF0aCwgb3B0aW9uc1xuXHQjIOato+WImeWIpOaWrVxuXHRyZXR1cm4gZmFsc2UgaWYgcmVnLnRlc3QgZnN0clxuXG5cdCMg5LiN6YeN5aSN5bCx5o+S5YWlXG5cdGF3YWl0IGZzLmFwcGVuZEZpbGUgcnBhdGgsIHN0ciA/IHJlZy5zb3VyY2UsIG9wdGlvbnNcblx0dHJ1ZVxuXG4jIOWIpOaWreaYr+aWh+S7ti/nm67lvZVcbmV4cG9ydCBmaWxldHlwZSA9IChwYXRoKS0+XG5cdHN0YXQ9YXdhaXQgZnMubHN0YXQgcmVhbHBhdGggcGF0aFxuXHRyZXR1cm4gaXNmaWxlOiBzdGF0LmlzRmlsZSgpLCBpc2Rpcjogc3RhdC5pc0RpcmVjdG9yeSgpXG5cbiMg5Yik5pat5paH5Lu2L+ebruW9leaYr+WQpuWtmOWcqFxuZXhwb3J0IGV4aXN0ID0gKHBhdGgpLT5cblx0ZnRzLmV4aXN0c1N5bmMgcmVhbHBhdGggcGF0aFxuXG4jIOW7uuS4gOS4quebruW9lVxuZXhwb3J0IG1rZGlyPShwYXRoKS0+XG5cdGF3YWl0IGZzLm1rZGlyIChyZWFscGF0aCBwYXRoKSxyZWN1cnNpdmU6dHJ1ZVxuXG4jIOWmguaenOS4jeWtmOWcqOWwseW7uuS4gOS4quebruW9lVxuZXhwb3J0IG5ld2Rpcj0ocGF0aCktPlxuXHRyZXR1cm4gZmFsc2UgaWYgZXhpc3QgcGF0aFxuXHRhd2FpdCBmcy5ta2RpciByZWFscGF0aCBwYXRoXG5cbiMg6K+75Y+W55uu5b2VXG5leHBvcnQgcmVhZGlyID0gKHBhdGgpLT5cblx0YXdhaXQgZnMucmVhZGRpciByZWFscGF0aCBwYXRoXG4jIOivu+WPluaWh+S7tlxuZXhwb3J0IHJlYWRmaWxlID0gKHBhdGgpLT5cblx0ZnN0cj1hd2FpdCBmcy5yZWFkRmlsZSAocmVhbHBhdGggcGF0aCksIGVuY29kaW5nOid1dGY4J1xuXG5cbiMgdG9kbyDor7vlj5bnm67lvZXkuK3miYDmnInpl67popgsIOa3seW6pumBjeWOhlxuZXhwb3J0IGFsbGZpbGUgPSAoe3BhdGgsIHJlZz1mYWxzZSxvcHRpb25zPWVuY29kaW5nOid1dGY4J30pLT5cblx0IyDlpoLmnpzmsqHkvKByZWcsIOmCo+S5iOWwseaYr+imgeaJgOacieaWh+S7tlxuXHRyZWFkZGlyKHBhdGgsIG9wdGlvbnMpXG5cblxuXG5cbmV4cG9ydCBkZWZhdWx0IHtcblx0c2F2ZWZpbGVcblx0Y292ZXJcblx0dW5pcXVlYWRkXG5cdGZpbGV0eXBlXG5cdGV4aXN0XG5cdG1rZGlyXG5cdHJlYWRpclxuXHRyZWFkZmlsZVxuXHRhbGxmaWxlXG59XG4iLCJleHBvcnQge2RlZmF1bHQgYXMgY29sb3J9IGZyb20gJy4vY29sb3IuY3MnXG5leHBvcnQge2RlZmF1bHQgYXMgcmFuZG9tfSBmcm9tIFx0Jy4vcmFuZG9tLmNzJ1xuZXhwb3J0IHtkZWZhdWx0IGFzIHRpbWV9IGZyb20gXHQnLi90aW1lLmNzJ1xuZXhwb3J0IHtkZWZhdWx0IGFzIGZpbGV9IGZyb20gXHQnLi9maWxlLmNzJ1xuIiwiXG4jIyNcbiog5ou/5Yiw5pW05pWw54mI5pys55qE6ZqP5py65pWwXG4qIOWboOS4unJhbmRvbeS4jeWMheWQqzEsIOWboOatpCwg6L+Z5Liq6ZqP5py65pWw5Lmf5LiN5YyF5ZCrbWF4XG4qIOWboOatpOS8oOWFpeaVsOe7hOeahOmVv+W6puato+WlveWQiOmAguS9nOS4uuaVsOe7hOeahOe0ouW8lVxuIyMjXG5pbXBvcnQge2xzdHJ9IGZyb20gJy4vY29sb3IuY3MnXG5cbmV4cG9ydCByYW5kb21pbnQ9KG1heCkgLT5cblx0KE1hdGgucmFuZG9tKCkqbWF4KT4+MFxuXG4jIyNcbuaVsOe7hOS4reWPluS4gOS4qumaj+acumVsZW1lbnRcbiMjI1xuZXhwb3J0IHJhbmRvbWVtYmVyID0gKHApIC0+XG5cdHBbcmFuZG9taW50IHAubGVuZ3RoXVxuXG4jIOmaj+acuuS4gOS4quminOiJslxuZXhwb3J0IHJhbmRvbWNvbG9yID0gLT5cblx0XCJyZ2IoXCIrIChyYW5kb21pbnQgMjU2KStcIixcIisocmFuZG9taW50IDI1NiApK1wiLFwiKyhyYW5kb21pbnQgMjU2KStcIilcIlxuXG5cbiMg6ZqP5py65LiA5Liq5piO5Lqu6aKc6ImyXG5leHBvcnQgcmFuZG9tbGlnaHQgPSAtPlxuXHRsc3RyXG5cdFx0c3RyOnJhbmRvbWNvbG9yIDBcblx0XHRpc2JhY2s6ZmFsc2VcblxuXG5cblxuZXhwb3J0IGRlZmF1bHQge1xuXHRyYW5kb21pbnRcblx0cmFuZG9tZW1iZXJcblx0cmFuZG9tY29sb3Jcblx0cmFuZG9tbGlnaHRcbn1cbiIsImV4cG9ydCBzbGVlcCA9IChtcykgLT5cblx0bmV3IFByb21pc2UgKHJlc29sdmUpIC0+XG5cdFx0c2V0VGltZW91dCByZXNvbHZlLCBtc1xuXG5cbmV4cG9ydCBnZXRoaG1tID0odGltZXN0YW1wPURhdGUubm93KCkpLT5cblx0dD1uZXcgRGF0ZSgrdGltZXN0YW1wKVxuXHR0LmdldEhvdXJzKCkrJzonK3QuZ2V0TWludXRlcygpXG5cbmV4cG9ydCBnZXRoaG1tc3MgPSh0aW1lc3RhbXA9RGF0ZS5ub3coKSktPlxuXHR0PW5ldyBEYXRlKCt0aW1lc3RhbXApXG5cdHQuZ2V0SG91cnMoKSsnOicrdC5nZXRNaW51dGVzKCkrJzonK3QuZ2V0U2Vjb25kcygpXG5cblxuXG5cbmV4cG9ydCBkZWZhdWx0IHtcblx0c2xlZXBcblx0Z2V0aGhtbVxuXHRnZXRoaG1tc3Ncbn1cbiIsIm1vZHVsZS5leHBvcnRzID0ge1wiYmxhY2tcIjpcIiMwMDBcIixcInNpbHZlclwiOlwiI2MwYzBjMFwiLFwiZ3JheVwiOlwiIzgwODA4MFwiLFwid2hpdGVcIjpcIiNmZmZcIixcIm1hcm9vblwiOlwiIzgwMDAwMFwiLFwicmVkXCI6XCIjZmYwMDAwXCIsXCJwdXJwbGVcIjpcIiM4MDAwODBcIixcImZ1Y2hzaWFcIjpcIiNmZjAwZmZcIixcImdyZWVuXCI6XCIjMDA4MDAwXCIsXCJsaW1lXCI6XCIjMDBmZjAwXCIsXCJvbGl2ZVwiOlwiIzgwODAwMFwiLFwieWVsbG93XCI6XCIjZmZmZjAwXCIsXCJuYXZ5XCI6XCIjMDAwMDgwXCIsXCJibHVlXCI6XCIjMDAwMGZmXCIsXCJ0ZWFsXCI6XCIjMDA4MDgwXCIsXCJhcXVhXCI6XCIjMDBmZmZmXCIsXCJvcmFuZ2VcIjpcIiNmZmE1MDBcIixcImFsaWNlYmx1ZVwiOlwiI2YwZjhmZlwiLFwiYW50aXF1ZXdoaXRlXCI6XCIjZmFlYmQ3XCIsXCJhcXVhbWFyaW5lXCI6XCIjN2ZmZmQ0XCIsXCJhenVyZVwiOlwiI2YwZmZmZlwiLFwiYmVpZ2VcIjpcIiNmNWY1ZGNcIixcImJpc3F1ZVwiOlwiI2ZmZTRjNFwiLFwiYmxhbmNoZWRhbG1vbmRcIjpcIiNmZmViY2RcIixcImJsdWV2aW9sZXRcIjpcIiM4YTJiZTJcIixcImJyb3duXCI6XCIjYTUyYTJhXCIsXCJidXJseXdvb2RcIjpcIiNkZWI4ODdcIixcImNhZGV0Ymx1ZVwiOlwiIzVmOWVhMFwiLFwiY2hhcnRyZXVzZVwiOlwiIzdmZmYwMFwiLFwiY2hvY29sYXRlXCI6XCIjZDI2OTFlXCIsXCJjb3JhbFwiOlwiI2ZmN2Y1MFwiLFwiY29ybmZsb3dlcmJsdWVcIjpcIiM2NDk1ZWRcIixcImNvcm5zaWxrXCI6XCIjZmZmOGRjXCIsXCJjcmltc29uXCI6XCIjZGMxNDNjXCIsXCJjeWFuXCI6XCIjMDBmZmZmXCIsXCJkYXJrYmx1ZVwiOlwiIzAwMDA4YlwiLFwiZGFya2N5YW5cIjpcIiMwMDhiOGJcIixcImRhcmtnb2xkZW5yb2RcIjpcIiNiODg2MGJcIixcImRhcmtncmF5XCI6XCIjYTlhOWE5XCIsXCJkYXJrZ3JlZW5cIjpcIiMwMDY0MDBcIixcImRhcmtncmV5XCI6XCIjYTlhOWE5XCIsXCJkYXJra2hha2lcIjpcIiNiZGI3NmJcIixcImRhcmttYWdlbnRhXCI6XCIjOGIwMDhiXCIsXCJkYXJrb2xpdmVncmVlblwiOlwiIzU1NmIyZlwiLFwiZGFya29yYW5nZVwiOlwiI2ZmOGMwMFwiLFwiZGFya29yY2hpZFwiOlwiIzk5MzJjY1wiLFwiZGFya3JlZFwiOlwiIzhiMDAwMFwiLFwiZGFya3NhbG1vblwiOlwiI2U5OTY3YVwiLFwiZGFya3NlYWdyZWVuXCI6XCIjOGZiYzhmXCIsXCJkYXJrc2xhdGVibHVlXCI6XCIjNDgzZDhiXCIsXCJkYXJrc2xhdGVncmF5XCI6XCIjMmY0ZjRmXCIsXCJkYXJrc2xhdGVncmV5XCI6XCIjMmY0ZjRmXCIsXCJkYXJrdHVycXVvaXNlXCI6XCIjMDBjZWQxXCIsXCJkYXJrdmlvbGV0XCI6XCIjOTQwMGQzXCIsXCJkZWVwcGlua1wiOlwiI2ZmMTQ5M1wiLFwiZGVlcHNreWJsdWVcIjpcIiMwMGJmZmZcIixcImRpbWdyYXlcIjpcIiM2OTY5NjlcIixcImRpbWdyZXlcIjpcIiM2OTY5NjlcIixcImRvZGdlcmJsdWVcIjpcIiMxZTkwZmZcIixcImZpcmVicmlja1wiOlwiI2IyMjIyMlwiLFwiZmxvcmFsd2hpdGVcIjpcIiNmZmZhZjBcIixcImZvcmVzdGdyZWVuXCI6XCIjMjI4YjIyXCIsXCJnYWluc2Jvcm9cIjpcIiNkY2RjZGNcIixcImdob3N0d2hpdGVcIjpcIiNmOGY4ZmZcIixcImdvbGRcIjpcIiNmZmQ3MDBcIixcImdvbGRlbnJvZFwiOlwiI2RhYTUyMFwiLFwiZ3JlZW55ZWxsb3dcIjpcIiNhZGZmMmZcIixcImdyZXlcIjpcIiM4MDgwODBcIixcImhvbmV5ZGV3XCI6XCIjZjBmZmYwXCIsXCJob3RwaW5rXCI6XCIjZmY2OWI0XCIsXCJpbmRpYW5yZWRcIjpcIiNjZDVjNWNcIixcImluZGlnb1wiOlwiIzRiMDA4MlwiLFwiaXZvcnlcIjpcIiNmZmZmZjBcIixcImtoYWtpXCI6XCIjZjBlNjhjXCIsXCJsYXZlbmRlclwiOlwiI2U2ZTZmYVwiLFwibGF2ZW5kZXJibHVzaFwiOlwiI2ZmZjBmNVwiLFwibGF3bmdyZWVuXCI6XCIjN2NmYzAwXCIsXCJsZW1vbmNoaWZmb25cIjpcIiNmZmZhY2RcIixcImxpZ2h0Ymx1ZVwiOlwiI2FkZDhlNlwiLFwibGlnaHRjb3JhbFwiOlwiI2YwODA4MFwiLFwibGlnaHRjeWFuXCI6XCIjZTBmZmZmXCIsXCJsaWdodGdvbGRlbnJvZHllbGxvd1wiOlwiI2ZhZmFkMlwiLFwibGlnaHRncmF5XCI6XCIjZDNkM2QzXCIsXCJsaWdodGdyZWVuXCI6XCIjOTBlZTkwXCIsXCJsaWdodGdyZXlcIjpcIiNkM2QzZDNcIixcImxpZ2h0cGlua1wiOlwiI2ZmYjZjMVwiLFwibGlnaHRzYWxtb25cIjpcIiNmZmEwN2FcIixcImxpZ2h0c2VhZ3JlZW5cIjpcIiMyMGIyYWFcIixcImxpZ2h0c2t5Ymx1ZVwiOlwiIzg3Y2VmYVwiLFwibGlnaHRzbGF0ZWdyYXlcIjpcIiM3Nzg4OTlcIixcImxpZ2h0c2xhdGVncmV5XCI6XCIjNzc4ODk5XCIsXCJsaWdodHN0ZWVsYmx1ZVwiOlwiI2IwYzRkZVwiLFwibGlnaHR5ZWxsb3dcIjpcIiNmZmZmZTBcIixcImxpbWVncmVlblwiOlwiIzMyY2QzMlwiLFwibGluZW5cIjpcIiNmYWYwZTZcIixcIm1hZ2VudGFcIjpcIiNmZjAwZmZcIixcIm1lZGl1bWFxdWFtYXJpbmVcIjpcIiM2NmNkYWFcIixcIm1lZGl1bWJsdWVcIjpcIiMwMDAwY2RcIixcIm1lZGl1bW9yY2hpZFwiOlwiI2JhNTVkM1wiLFwibWVkaXVtcHVycGxlXCI6XCIjOTM3MGRiXCIsXCJtZWRpdW1zZWFncmVlblwiOlwiIzNjYjM3MVwiLFwibWVkaXVtc2xhdGVibHVlXCI6XCIjN2I2OGVlXCIsXCJtZWRpdW1zcHJpbmdncmVlblwiOlwiIzAwZmE5YVwiLFwibWVkaXVtdHVycXVvaXNlXCI6XCIjNDhkMWNjXCIsXCJtZWRpdW12aW9sZXRyZWRcIjpcIiNjNzE1ODVcIixcIm1pZG5pZ2h0Ymx1ZVwiOlwiIzE5MTk3MFwiLFwibWludGNyZWFtXCI6XCIjZjVmZmZhXCIsXCJtaXN0eXJvc2VcIjpcIiNmZmU0ZTFcIixcIm1vY2Nhc2luXCI6XCIjZmZlNGI1XCIsXCJuYXZham93aGl0ZVwiOlwiI2ZmZGVhZFwiLFwib2xkbGFjZVwiOlwiI2ZkZjVlNlwiLFwib2xpdmVkcmFiXCI6XCIjNmI4ZTIzXCIsXCJvcmFuZ2VyZWRcIjpcIiNmZjQ1MDBcIixcIm9yY2hpZFwiOlwiI2RhNzBkNlwiLFwicGFsZWdvbGRlbnJvZFwiOlwiI2VlZThhYVwiLFwicGFsZWdyZWVuXCI6XCIjOThmYjk4XCIsXCJwYWxldHVycXVvaXNlXCI6XCIjYWZlZWVlXCIsXCJwYWxldmlvbGV0cmVkXCI6XCIjZGI3MDkzXCIsXCJwYXBheWF3aGlwXCI6XCIjZmZlZmQ1XCIsXCJwZWFjaHB1ZmZcIjpcIiNmZmRhYjlcIixcInBlcnVcIjpcIiNjZDg1M2ZcIixcInBpbmtcIjpcIiNmZmMwY2JcIixcInBsdW1cIjpcIiNkZGEwZGRcIixcInBvd2RlcmJsdWVcIjpcIiNiMGUwZTZcIixcInJvc3licm93blwiOlwiI2JjOGY4ZlwiLFwicm95YWxibHVlXCI6XCIjNDE2OWUxXCIsXCJzYWRkbGVicm93blwiOlwiIzhiNDUxM1wiLFwic2FsbW9uXCI6XCIjZmE4MDcyXCIsXCJzYW5keWJyb3duXCI6XCIjZjRhNDYwXCIsXCJzZWFncmVlblwiOlwiIzJlOGI1N1wiLFwic2Vhc2hlbGxcIjpcIiNmZmY1ZWVcIixcInNpZW5uYVwiOlwiI2EwNTIyZFwiLFwic2t5Ymx1ZVwiOlwiIzg3Y2VlYlwiLFwic2xhdGVibHVlXCI6XCIjNmE1YWNkXCIsXCJzbGF0ZWdyYXlcIjpcIiM3MDgwOTBcIixcInNsYXRlZ3JleVwiOlwiIzcwODA5MFwiLFwic25vd1wiOlwiI2ZmZmFmYVwiLFwic3ByaW5nZ3JlZW5cIjpcIiMwMGZmN2ZcIixcInN0ZWVsYmx1ZVwiOlwiIzQ2ODJiNFwiLFwidGFuXCI6XCIjZDJiNDhjXCIsXCJ0aGlzdGxlXCI6XCIjZDhiZmQ4XCIsXCJ0b21hdG9cIjpcIiNmZjYzNDdcIixcInRyYW5zcGFyZW50XCI6XCIjMDAwMFwiLFwidHVycXVvaXNlXCI6XCIjNDBlMGQwXCIsXCJ2aW9sZXRcIjpcIiNlZTgyZWVcIixcIndoZWF0XCI6XCIjZjVkZWIzXCIsXCJ3aGl0ZXNtb2tlXCI6XCIjZjVmNWY1XCIsXCJ5ZWxsb3dncmVlblwiOlwiIzlhY2QzMlwiLFwicmViZWNjYXB1cnBsZVwiOlwiIzY2MzM5OVwifSIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcIkBiYWJlbC9ydW50aW1lL2hlbHBlcnMvYXN5bmNUb0dlbmVyYXRvclwiKTs7IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy9pbnRlcm9wUmVxdWlyZURlZmF1bHRcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcIkBiYWJlbC9ydW50aW1lL3JlZ2VuZXJhdG9yXCIpOzsiLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJjaGFsa1wiKTs7IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiZnNcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcImZzL3Byb21pc2VzXCIpOzsiLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJvc1wiKTs7Il0sInNvdXJjZVJvb3QiOiIifQ==