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
exports.id = "bin";
exports.ids = ["bin"];
exports.modules = {

/***/ "./src/lornpug.cs":
/*!************************!*
  !*** ./src/lornpug.cs ***!
  \************************/
/***/ ((__unused_webpack_module, __unused_webpack_exports, __webpack_require__) => {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "@babel/runtime/helpers/interopRequireDefault");

var _chalk = _interopRequireDefault(__webpack_require__(/*! chalk */ "chalk"));

var _lornpughelp = _interopRequireDefault(__webpack_require__(/*! ./lornpughelp.cs */ "./src/lornpughelp.cs"));

var arg, cmds, dodir, dolp, files, i, iscmd, isdefault, isdir, isio, islp, j, k, len, len1, len2, x;
iscmd = _lornpughelp["default"].iscmd;
dolp = _lornpughelp["default"].dolp;
dodir = _lornpughelp["default"].dodir;
// ------------------init---------------------
arg = process.argv.slice(2);
console.log('环境和参数信息:');
console.table([{
  '键: ': 'process.cwd()',
  '值: ': process.cwd()
}, {
  '键: ': '__dirname',
  '值: ': __dirname
}, {
  '键: ': 'process.argv',
  '值: ': process.argv
}, {
  '键: ': '真参数',
  '值: ': arg
}]);
cmds = []; // 传入的参数的命令数组

files = []; // 传入的文件数组

for (i = 0, len = arg.length; i < len; i++) {
  x = arg[i];

  if (iscmd(x)) {
    cmds.push(x);
  } else {
    files.push(x);
  }
}

console.log(_chalk["default"].blueBright("\u547D\u4EE4:  ".concat(cmds, " ")));
console.log(_chalk["default"].yellowBright("\u6587\u4EF6: ".concat(files, " "))); // 处理命令

isdefault = cmds.length === 0;

for (j = 0, len1 = cmds.length; j < len1; j++) {
  x = cmds[j];
  isdir = /^--dir$/.test(x);
  isio = /^--io$/.test(x);
  islp = /^--lp$/.test(x);
} // 根据命令, 处理文件, 忽略node_module, 未来根据.gitignore忽略


if (isdefault) {
  // 默认是跑测试文件
  console.log(_chalk["default"].cyanBright('默认行为:'), '测试结果: ');
} else if (islp) {
  // 修改.zshrc文件, 增加快捷方式
  dolp(0);
} else if (isdir) {
  // 建测试目录, 初始化测试文件
  if (files.length === 0) {
    dodir('.'); //如果没有任何文件参数, 那么就是从本地搞深度遍历
  } else {
    for (k = 0, len2 = files.length; k < len2; k++) {
      x = files[k];
      dodir(x);
    }
  }

  console.log(_chalk["default"].cyanBright('初始化'), '初始化成功: ');
} else if (isio) {
  // 增加测试文件
  console.log(_chalk["default"].cyanBright('初始化'), '初始化成功: ');
} else {
  console.log(_chalk["default"].redBright('抱歉啊: 不知道你想干啥'));
}
/*
try {
	new Function('var {a} = {a: 1}')();
} catch(error) {
	console.error('Your JavaScript runtime does not support some features used by the coffee command. Please use Node 6 or later.');
	process.exit(1);
}

var path=require('path');
var fs=require('fs');

var potentialPaths=[
	path.join(process.cwd(),),
	path.join(process.cwd(),),
	path.join(process.cwd(),),
	path.join(__dirname,'../lib/c')
];

for(var i=0,len=potentialPaths.length;i<len;i++) {
	if(fs.existsSync(potentialPaths[i])) {
		require(potentialPaths[i]+'/command').run();
		break;
	}
}

//传参数
var program=require('commander');

program
	.arguments('<file>')
	.option('-u, --username <username>','The user to authenticate as')
	.option('-p, --password <password>','The users password')
		.action(function(file) {
			console.log('user: %s pass: %s file: %s',
				program.username,program.password,file);
		})
		.parse(process.argv);
*/

/***/ }),

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
return __webpack_require__.X([], "./src/lornpug.cs");
})();
});
//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly9tbGliL3dlYnBhY2svdW5pdmVyc2FsTW9kdWxlRGVmaW5pdGlvbiIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL2xvcm5wdWcuY3MiLCJ3ZWJwYWNrOi8vbWxpYi8uL3NyYy9sb3JucHVnaGVscC5jcyIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL21saWIvc3JjL2NvbG9yLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvZmlsZS5jcyIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL21saWIvc3JjL2luZGV4LmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvcmFuZG9tLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvdGltZS5jcyIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL21saWIvc3JjL2NvbG9ybmFtZS5jc29uIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJAYmFiZWwvcnVudGltZS9oZWxwZXJzL2FzeW5jVG9HZW5lcmF0b3JcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy9pbnRlcm9wUmVxdWlyZURlZmF1bHRcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiQGJhYmVsL3J1bnRpbWUvcmVnZW5lcmF0b3JcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiY2hhbGtcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiZnNcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiZnMvcHJvbWlzZXNcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwib3NcIiJdLCJuYW1lcyI6W10sIm1hcHBpbmdzIjoiO0FBQUE7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0EsQ0FBQztBQUNELE87Ozs7Ozs7Ozs7Ozs7Ozs7O0FDVkE7O0FBR0E7O0FBSEE7QUFLQSxLLEdBTEEsdUIsQ0FLQSxLO0FBQUEsSSxHQUxBLHVC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QUFELFk7QUFGRixDQUFoQjs7OztBQUlBLElBQU8sWUFBVyxTQUFYLFNBQVc7QUFBQSxNQUFDLFNBQUQsdUVBQVcsSUFBSSxDQUFmLEdBQVcsRUFBWDtBQUNsQjtBQUFDLE1BQUUsU0FBUyxDQUFULFVBQUY7U0FDQSxDQUFDLENBQUQsbUJBQWlCLENBQUMsQ0FBbEIsVUFBaUIsRUFBakIsU0FBb0MsQ0FBQyxDQUFELFk7QUFGbkIsQ0FBbEI7OztlQU9lO0FBQ2QsT0FEYyxFQUNkLEtBRGM7QUFFZCxTQUZjLEVBRWQsT0FGYztBQUdkLFdBSGMsRUFHZDtBQUhjLEM7Ozs7Ozs7Ozs7O0FDaEJmLGtCQUFrQiw4ckc7Ozs7Ozs7Ozs7O0FDQWxCLHFFOzs7Ozs7Ozs7OztBQ0FBLDBFOzs7Ozs7Ozs7OztBQ0FBLHdEOzs7Ozs7Ozs7OztBQ0FBLG1DOzs7Ozs7Ozs7OztBQ0FBLGdDOzs7Ozs7Ozs7OztBQ0FBLHlDOzs7Ozs7Ozs7OztBQ0FBLGdDIiwiZmlsZSI6ImJpbi9iaW4uanMiLCJzb3VyY2VzQ29udGVudCI6WyIoZnVuY3Rpb24gd2VicGFja1VuaXZlcnNhbE1vZHVsZURlZmluaXRpb24ocm9vdCwgZmFjdG9yeSkge1xuXHRpZih0eXBlb2YgZXhwb3J0cyA9PT0gJ29iamVjdCcgJiYgdHlwZW9mIG1vZHVsZSA9PT0gJ29iamVjdCcpXG5cdFx0bW9kdWxlLmV4cG9ydHMgPSBmYWN0b3J5KCk7XG5cdGVsc2UgaWYodHlwZW9mIGRlZmluZSA9PT0gJ2Z1bmN0aW9uJyAmJiBkZWZpbmUuYW1kKVxuXHRcdGRlZmluZShbXSwgZmFjdG9yeSk7XG5cdGVsc2UgaWYodHlwZW9mIGV4cG9ydHMgPT09ICdvYmplY3QnKVxuXHRcdGV4cG9ydHNbXCJtbGliXCJdID0gZmFjdG9yeSgpO1xuXHRlbHNlXG5cdFx0cm9vdFtcIm1saWJcIl0gPSBmYWN0b3J5KCk7XG59KShnbG9iYWwsIGZ1bmN0aW9uKCkge1xucmV0dXJuICIsImltcG9ydCBjaGFsayBmcm9tICdjaGFsaydcbiNjaGFsayA9IHJlcXVpcmUoJ2NoYWxrJylcbiNpbXBvcnQge2ZpbGV9IGZyb20gJ21saWInXG5pbXBvcnQgbHBoIGZyb20gJy4vbG9ybnB1Z2hlbHAuY3MnXG5cbntpc2NtZCwgZG9scCwgZG9kaXJ9PWxwaFxuXG5cbiMgLS0tLS0tLS0tLS0tLS0tLS0taW5pdC0tLS0tLS0tLS0tLS0tLS0tLS0tLVxuYXJnPXByb2Nlc3MuYXJndlsyLi5dXG5jb25zb2xlLmxvZyAn546v5aKD5ZKM5Y+C5pWw5L+h5oGvOidcbmNvbnNvbGUudGFibGUgW1xuXHR7J+mUrjogJzoncHJvY2Vzcy5jd2QoKScsICflgLw6ICc6IHByb2Nlc3MuY3dkKCl9XG5cdHsn6ZSuOiAnOidfX2Rpcm5hbWUnLCAn5YC8OiAnOl9fZGlybmFtZX1cblx0eyfplK46ICc6J3Byb2Nlc3MuYXJndicsICflgLw6ICc6cHJvY2Vzcy5hcmd2fVxuXHR7J+mUrjogJzon55yf5Y+C5pWwJywgJ+WAvDogJzphcmd9XG5dXG5cbmNtZHM9W10gIyDkvKDlhaXnmoTlj4LmlbDnmoTlkb3ku6TmlbDnu4RcbmZpbGVzPVtdICMg5Lyg5YWl55qE5paH5Lu25pWw57uEXG5cbmZvciB4IGluIGFyZ1xuXHRpZiBpc2NtZCB4IHRoZW4gY21kcy5wdXNoIHggZWxzZSBmaWxlcy5wdXNoIHhcblxuXG5jb25zb2xlLmxvZyBjaGFsay5ibHVlQnJpZ2h0IFwi5ZG95LukOiAgI3tjbWRzfSBcIlxuY29uc29sZS5sb2cgY2hhbGsueWVsbG93QnJpZ2h0IFwi5paH5Lu2OiAje2ZpbGVzfSBcIlxuXG5cbiMg5aSE55CG5ZG95LukXG5pc2RlZmF1bHQgPSBjbWRzLmxlbmd0aCA9PTBcbmZvciB4IGluIGNtZHNcblx0aXNkaXI9L14tLWRpciQvLnRlc3QgeFxuXHRpc2lvPS9eLS1pbyQvLnRlc3QgeFxuXHRpc2xwPS9eLS1scCQvLnRlc3QgeFxuXG5cblxuXG4jIOagueaNruWRveS7pCwg5aSE55CG5paH5Lu2LCDlv73nlaVub2RlX21vZHVsZSwg5pyq5p2l5qC55o2uLmdpdGlnbm9yZeW/veeVpVxuaWYgaXNkZWZhdWx0ICMg6buY6K6k5piv6LeR5rWL6K+V5paH5Lu2XG5cdGNvbnNvbGUubG9nIChjaGFsay5jeWFuQnJpZ2h0ICfpu5jorqTooYzkuLo6JyksICfmtYvor5Xnu5Pmnpw6ICdcblxuZWxzZSBpZiBpc2xwICMg5L+u5pS5LnpzaHJj5paH5Lu2LCDlop7liqDlv6vmjbfmlrnlvI9cblx0ZG9scCAwXG5lbHNlIGlmIGlzZGlyICMg5bu65rWL6K+V55uu5b2VLCDliJ3lp4vljJbmtYvor5Xmlofku7Zcblx0aWYgZmlsZXMubGVuZ3RoPT0wXG5cdFx0ZG9kaXIgJy4nICPlpoLmnpzmsqHmnInku7vkvZXmlofku7blj4LmlbAsIOmCo+S5iOWwseaYr+S7juacrOWcsOaQnua3seW6pumBjeWOhlxuXHRlbHNlXG5cdFx0ZG9kaXIgeCBmb3IgeCBpbiBmaWxlc1xuXHRcdFxuXHRjb25zb2xlLmxvZyAoY2hhbGsuY3lhbkJyaWdodCAn5Yid5aeL5YyWJyksICfliJ3lp4vljJbmiJDlip86ICdcblxuZWxzZSBpZiBpc2lvICMg5aKe5Yqg5rWL6K+V5paH5Lu2XG5cdGNvbnNvbGUubG9nIChjaGFsay5jeWFuQnJpZ2h0ICfliJ3lp4vljJYnKSwgJ+WIneWni+WMluaIkOWKnzogJ1xuXHRcbmVsc2Vcblx0Y29uc29sZS5sb2cgY2hhbGsucmVkQnJpZ2h0ICfmirHmrYnllYo6IOS4jeefpemBk+S9oOaDs+W5suWVpSdcblxuXG5cblxuIyMjXG50cnkge1xuXHRuZXcgRnVuY3Rpb24oJ3ZhciB7YX0gPSB7YTogMX0nKSgpO1xufSBjYXRjaChlcnJvcikge1xuXHRjb25zb2xlLmVycm9yKCdZb3VyIEphdmFTY3JpcHQgcnVudGltZSBkb2VzIG5vdCBzdXBwb3J0IHNvbWUgZmVhdHVyZXMgdXNlZCBieSB0aGUgY29mZmVlIGNvbW1hbmQuIFBsZWFzZSB1c2UgTm9kZSA2IG9yIGxhdGVyLicpO1xuXHRwcm9jZXNzLmV4aXQoMSk7XG59XG5cbnZhciBwYXRoPXJlcXVpcmUoJ3BhdGgnKTtcbnZhciBmcz1yZXF1aXJlKCdmcycpO1xuXG52YXIgcG90ZW50aWFsUGF0aHM9W1xuXHRwYXRoLmpvaW4ocHJvY2Vzcy5jd2QoKSwpLFxuXHRwYXRoLmpvaW4ocHJvY2Vzcy5jd2QoKSwpLFxuXHRwYXRoLmpvaW4ocHJvY2Vzcy5jd2QoKSwpLFxuXHRwYXRoLmpvaW4oX19kaXJuYW1lLCcuLi9saWIvYycpXG5dO1xuXG5mb3IodmFyIGk9MCxsZW49cG90ZW50aWFsUGF0aHMubGVuZ3RoO2k8bGVuO2krKykge1xuXHRpZihmcy5leGlzdHNTeW5jKHBvdGVudGlhbFBhdGhzW2ldKSkge1xuXHRcdHJlcXVpcmUocG90ZW50aWFsUGF0aHNbaV0rJy9jb21tYW5kJykucnVuKCk7XG5cdFx0YnJlYWs7XG5cdH1cbn1cblxuXG4vL+S8oOWPguaVsFxudmFyIHByb2dyYW09cmVxdWlyZSgnY29tbWFuZGVyJyk7XG5cbnByb2dyYW1cblx0LmFyZ3VtZW50cygnPGZpbGU+Jylcblx0Lm9wdGlvbignLXUsIC0tdXNlcm5hbWUgPHVzZXJuYW1lPicsJ1RoZSB1c2VyIHRvIGF1dGhlbnRpY2F0ZSBhcycpXG5cdC5vcHRpb24oJy1wLCAtLXBhc3N3b3JkIDxwYXNzd29yZD4nLCdUaGUgdXNlcnMgcGFzc3dvcmQnKVxuXHRcdC5hY3Rpb24oZnVuY3Rpb24oZmlsZSkge1xuXHRcdFx0Y29uc29sZS5sb2coJ3VzZXI6ICVzIHBhc3M6ICVzIGZpbGU6ICVzJyxcblx0XHRcdFx0cHJvZ3JhbS51c2VybmFtZSxwcm9ncmFtLnBhc3N3b3JkLGZpbGUpO1xuXHRcdH0pXG5cdFx0LnBhcnNlKHByb2Nlc3MuYXJndik7XG4jIyNcbiIsImltcG9ydCBjaGFsayBmcm9tICdjaGFsaydcbiNjaGFsayA9IHJlcXVpcmUoJ2NoYWxrJylcbmltcG9ydCB7ZmlsZX0gZnJvbSAnbWxpYidcblxuIyAtLS0tLS0tLS0tLS0tLS0tLXB1cmUgZnVuY3Rpb24tLS0tLS0tLS0tLS0tLS0tLVxuIyDliKTmlq3mmK/lkKbmnInlkb3ku6Tlj4LmlbAoLS3lvIDlp4vnmoTlkb3ku6Tlj4LmlbApLCDmsqHmnIktLeeahOmDveaYr+aWh+S7tlxuZXhwb3J0IGlzY21kPSAoeCktPlxuXHQvXi0tLy50ZXN0IHhcblxuIyDlpITnkIZscOWRveS7pFxuZXhwb3J0IGRvbHAgPSAtPlxuXHRzdHI9JycnXG5cdCMg6IiU54uX6aG555uuXG5cdGFsaWFzIGxwPVwibHBnXCJcblx0JycnXG5cdHJlZz0vYWxpYXNcXHMqbHA9XCJscGdcIi9cblx0b2s9YXdhaXQgZmlsZS51bmlxdWVhZGQge1xuXHRcdHJlZ1xuXHRcdHN0clxuXHRcdHBhdGg6J34vLnpzaHJjJ1xuXHRcdCNwYXRoOiAnUkVBRE1FLm1kJ1xuXHR9XG5cdGlmIG9rXG5cdFx0Y29uc29sZS5sb2cgKCAn5ZG95Luk6K6+572u5Li6bHAnKSwgKGNoYWxrLmdyZWVuQnJpZ2h0ICfmiJDlip86ICcpLCfpnIDopoHliLfmlrDphY3nva7miY3kvJrnlJ/mlYgsIOaOqOiNkOS9v+eUqOW/q+aNt+mHjeWQr+WRveS7pDonXG5cdFx0Y29uc29sZS5sb2cgKGNoYWxrLmdyZWVuQnJpZ2h0ICdleGVjIHpzaCcpLChjaGFsay55ZWxsb3cgJyMg6YeN5ZCv5b2T5YmNdGVybWluYWwnKVxuXG5cdFx0Y29uc29sZS5sb2cgICflpoLmnpzkuI3og73ph43lkK90ZXJtaW5hbDogJ1xuXHRcdGNvbnNvbGUuIGxvZyAoY2hhbGsuZ3JlZW5CcmlnaHQgJ3NvdXJjZSB+Ly56c2hyYycpLCAoY2hhbGsueWVsbG93ICcjIOWvvOWFpemFjee9ruaWh+S7tuaWueazleS4gCcpXG5cdFx0Y29uc29sZS4gbG9nIChjaGFsay5ncmVlbkJyaWdodCAnLiB+Ly56c2hyYycpLCAoY2hhbGsueWVsbG93ICcjIOWvvOWFpemFjee9ruaWh+S7tuaWueazleS6jCcpXG5cdGVsc2Vcblx0XHRjb25zb2xlLmxvZyAoY2hhbGsucmVkQnJpZ2h0ICfmsqHmnIknKSwgKCAn6L+b6KGM6K6+572uLCDlj6/og73mmK/phY3nva7mlofku7blt7Lnu4/lrZjlnKgnKSwgKGNoYWxrLnllbGxvdyAn6K+35omL5bel5qOA5p+l5paH5Lu2OiAnKSwnfi8uenNocmMnXG5cbiMg5aSE55CG5LiA5LiqY3PmiJbogIVjb2ZmZWXnmoTliJ3lp4vljJZcbmRpcjFmaWxlID0gKHBhdGgpLT5cblx0cD0gcGF0aCsnLmxwJ1xuXHQjIOWIpOaWreWvueW6lOebruW9leaYr+WQpuWtmOWcqC5cblx0cmV0dXJuIGlmIChmaWxlLmV4aXN0IHApXG5cdCMg5paw5bu655uu5b2VXG5cdGF3YWl0IGZpbGUubWtkaXIgcFxuXHQjIHRvZG/or7vlj5bmlofku7YsIOWIhuaekOWHveaVsCwg5bu656uL5a+55bqU55qE5rWL6K+V5paH5Lu2XG5cdCMg5Yid5aeL5YyW5rWL6K+V5paH5Lu2XG5cdGF3YWl0IGZpbGUuY292ZXIgcGF0aDpwKycvYWFhLjBpLmNzb24nLHN0cjonJ1xuXHRhd2FpdCBmaWxlLmNvdmVyIHBhdGg6cCsnL2FhYS4wby5jc29uJyxzdHI6Jydcblx0YXdhaXQgZmlsZS5jb3ZlciBwYXRoOnArJy9jdG8uY3Nvbicsc3RyOicnXG5cbiMg5aSE55CG5Yid5aeL5YyW5rWL6K+VZGlyXG5leHBvcnQgZG9kaXIgPSAocGF0aCktPlxuXHRzdGF0PSBhd2FpdCBmaWxlLmZpbGV0eXBlIHBhdGhcblx0aWYgc3RhdC5pc2ZpbGUgIyDmlofku7blpITnkIZcblx0XHQjIOWkhOeQhmNvZmZlZeWSjGNzXG5cdFx0cmV0dXJuIHVubGVzcyAvXFwuY29mZmVlJHxcXC5jcyQvLnRlc3QgcGF0aFxuXHRcdGRpcjFmaWxlIHBhdGhcblx0aWYgc3RhdC5pc2RpciAjIOebruW9leWkhOeQhlxuXHRcdCMg6YCS5b2S5aSE55CG55uu5b2VLCDopoHpgb/lvIDoh6rliqjnlJ/miJDnmoTnm67lvZUo5ZCN56ew6YeM6Z2i5bim54K555qEKVxuXHRcdHJldHVybiBpZiAvXFwuLy50ZXN0IHBhdGhcblx0XHQjIOivu+WPluebruW9lSwg6YCQ6aG55aSE55CGXG5cdFx0ZG9kaXIgeCBmb3IgeCBpbiBhd2FpdCBmaWxlLnJlYWRpciBwYXRoXG5cdHJldHVybiAjIOmZpOS6hmZpbGXlkoxkaXLlj6/og73ov5jmnInlhbbku5bmg4XlhrUsIOavlOWmguespuWPt+mTvuaOpVxuXG5leHBvcnQgZGVmYXVsdCB7XG5cdGlzY21kXG5cdGRvbHBcblx0ZG9kaXJcbn1cblxuZG9kaXIgJy4nXG4iLCIjIyNcbiAqIOWFs+S6juminOiJsueahOW6kywg6L+Z5Liq5bqT5Lya6KKr6YeN5YaZLCDlm6DkuLpoc2znmoTkuq7luqbliKTmlq3lj6/ku6XorqTkuLrmmK/plJnnmoQuIOaJgOS7peWPr+S7peS/neeVmWhzbOebuOWFs+eahOWHveaVsCwg5L2G5pivLCDlrp7pmYXlupTnlKjlh73mlbDpnIDopoHph43lhpkuXG4gKlxuIyMjXG5pbXBvcnQgY29sb3JuYW1lIGZyb20gJy4vY29sb3JuYW1lLmNzb24nXG5cbmRhcmtjb2xvcj0gJ3JnYmEoNDQsNDQsNDQsMSknICNkYXJr55qE5Z+656GA6aKc6ImyXG5saWdodGNvbG9yPSAncmdiYSgyMDAsMjAwLDIwMCwxKSdcblxuXG5cblxuXG4jIyMqXG4gKiAtLS0tLS0tLS0tLS0tLS0tLS0tLS3ljZXni6zlpITnkIbliY3mma/miJbogIXog4zmma8sIOmFjeWQiGRhcmtjc3PnmoTmgJ3ot68tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS1cbiAqIOS6ruW6puWIpOaWremAu+i+kSBnPjEwMCwgcj4yMDAsIOaIluiAhWcrcj4zMDBcbiAqIOS6ruW6puWkhOeQhumAu+i+kSB0PW1heCgxMDAtZywgMjAwLXIpLCB0K3IsIHQrZywgdCtiXG4gKlxuICog5pqX5bqm5Yik5pat6YC76L6RIGc8NjAgcjwxMDAgYjwxNTBcbiAqIOaal+W6puWkhOeQhumAu+i+kSB0PW1heChnLTYwLCByLTEwMCwgYi0xNTApLCByLXQsIGctdCwgYi10XG4gKlxuICMjI1xuXG5cdCMjIypcblx0ICogcmdib2tcblx0ICog5Yik5pat6aKc6Imy5piv5ZCm6ZyA6KaB5aSE55CGXG5cdCAqICAqIOS6ruW6puWIpOaWremAu+i+kSBnPjEwMCwgcj4yMDAsIOaIluiAhWcrcj4zMDBcblx0ICogICog5pqX5bqm5Yik5pat6YC76L6RIGc8NjAgcjwxMDAgYjwxNTBcblx0ICog5Y+C5pWwXG5cdCAqIFx0cmdiOntyLGcsYn1cblx0ICogIGlzYmFjazog5piv5ZCm5Li66IOM5pmv5Zu+54mHXG5cdCAqIOi/lOWbnuWAvFxuXHQgKiBcdFx0dHJ1ZTog5LiN6ZyA6KaB5aSE55CGXG5cdCAqIFx0XHRmYWxzZTog6ZyA6KaB5aSE55CGXG5cdCAjIyNcbmV4cG9ydCByZ2Jvaz0oe3JnYixpc2JhY2s9dHJ1ZX0pLT5cblx0e3IsZyxifT1yZ2Jcblx0aWYgaXNiYWNrIHRoZW4gXHRnPDYwICYmIHI8MTAwICYmIGI8MTUwIGVsc2UgcitnPjMwMFxuXHRcdFx0I+aal+W6puWIpOaWrVxuXHRcdFx0I+WFieaYjuaooeW8jywg5aaC5p6c5Lqu5bqmPDAuN+WImemcgOimgeWkhOeQhiwg5ZCm5YiZ5bCx5LiN6ZyA6KaB5aSE55CGXG5cblxuIyMjKlxuICog5LuOc3Ry5Yiw6LCD5Lqu55qE6aKc6Imyc3RyXG4gKiAg6L+Z6YeM5aSE55CG5LiA5Liqc3R5bGUgc3RyaW5n55qE5Lqu5bqmXG4gKiDnuq/lh73mlbBcbiAqIHN0cjog5LiA5Liq6aKc6Imy5a2X56ym5LiyXG4gKiBpc2JhY2s6IOaYr+WQpuS4uuiDjOaZr1xuICog5bqf5byDIHJldHVybiBmYWxzZSDlubbkuI3lkIjnkIZcbiMjI1xuZXhwb3J0IGxzdHI9KHtzdHIsaXNiYWNrPXRydWV9KSAtPlxuXHRyZXR1cm4gZmFsc2UgaWYoIXN0cilcblx0XG5cdCPlpoLmnpzmmK9pbml0aWFsLCDpgqPkuYjnm7TmjqXnlKjmiJHnmoTlgLwuXG5cdHJldHVybiAoaWYgaXNiYWNrIHRoZW4gZGFya2NvbG9yIGVsc2UgbGlnaHRjb2xvcikgaWYgc3RyPT1cImluaXRpYWxcIlxuXHRcblx0I2ZpeG1lIOaaguaXtuWcqOi/memHjOW/veeVpWNzcyB2YXIgICN0b2RvXG5cdHJldHVybiAoaWYgaXNiYWNrIHRoZW4gZGFya2NvbG9yIGVsc2UgbGlnaHRjb2xvcilpZiAvdmFyXFwoLS0vLnRlc3Qoc3RyKVxuXG5cdGw9bHJnYiB7cmdiOiBzdHIycmdiKHN0ciksaXNiYWNrfVxuXHRyZ2Iyc3RyKGwpXG5cbiMjIypcblx0ICogbGlnaHRyZ2LmoLjlv4Pkuq7luqblpITnkIblh73mlbBcblx0ICogICog5Lqu5bqm5aSE55CG6YC76L6RIHQ9bWF4KDEwMC1nLCAyMDAtciksIHQrciwgdCtnLCB0K2Jcblx0ICogICog5pqX5bqm5aSE55CG6YC76L6RIHQ9bWF4KGctNjAsIHItMTAwLCBiLTE1MCksIHItdCwgZy10LCBiLXRcblx0ICog5bqf5byDcmV0dXJuIGZhbHNl5bm25LiN5ZCI55CGXG4jIyNcbmV4cG9ydCBscmdiPSh7cmdiLGlzYmFjaz10cnVlfSkgLT5cblx0cmV0dXJuIHJnYiBpZiByZ2JvayB7cmdiLGlzYmFja31cblx0e3IsZyxifT1yZ2Jcblx0aWYgKGlzYmFjaykgICPlpITnkIbmmpfpu5Hog4zmma9cblx0XHR0PSBNYXRoLm1heChnLTYwLHItMTAwLCBiLTE1MClcblx0XHRyLT10XG5cdFx0Zy09dFxuXHRcdGItPXRcblx0ZWxzZSAgI+WkhOeQhuaYjuS6ruWJjeaZr1xuXHRcdHQ9IE1hdGgubWF4KDEwMC1nLDIwMC1yKVxuXHRcdHIrPXRcblx0XHRnKz10XG5cdFx0Yis9dFxuXHR7cixnLGJ9XG5cbiMjIypcbiAqIOS7jnN0cuWIsOiwg+S6rueahOminOiJsnN0clxuICogIOi/memHjOWkhOeQhuS4gOS4qnN0eWxlIHN0cmluZ+eahOS6ruW6plxuICog57qv5Ye95pWwXG4gKiBzdHI6IOS4gOS4quminOiJsuWtl+espuS4slxuICogaXNiYWNrOiDmmK/lkKbkuLrog4zmma9cbiAqIOW6n+W8gyByZXR1cm4gZmFsc2Ug5bm25LiN5ZCI55CGXG4jIyNcbmxpZ2h0c3RyPSh7c3RyLGlzYmFjaz10cnVlfSkgLT5cblx0cmV0dXJuIGZhbHNlIGlmKCFzdHIpXG5cdFxuXHQj5aaC5p6c5pivaW5pdGlhbCwg6YKj5LmI55u05o6l55So5oiR55qE5YC8LlxuXHRyZXR1cm4gKGlmIGlzYmFjayB0aGVuIGRhcmtjb2xvciBlbHNlIGxpZ2h0Y29sb3IpIGlmIHN0cj09XCJpbml0aWFsXCJcblx0XG5cdCNmaXhtZSDmmoLml7blnKjov5nph4zlv73nlaVjc3MgdmFyICAjdG9kb1xuXHRyZXR1cm4gKGlmIGlzYmFjayB0aGVuIGRhcmtjb2xvciBlbHNlIGxpZ2h0Y29sb3IpaWYgL3ZhclxcKC0tLy50ZXN0KHN0cilcblx0bD1saWdodHJnYiB7cmdiOiBzdHIycmdiKHN0ciksaXNiYWNrfVxuXHRpZiBsIHRoZW4gcmdiMnN0cihsKSBlbHNlIGZhbHNlXG5cblxuXG4jIyMqXG5cdCAqIGxpZ2h0cmdi5qC45b+D5Lqu5bqm5aSE55CG5Ye95pWwXG5cdCAqICAqIOS6ruW6puWkhOeQhumAu+i+kSB0PW1heCgxMDAtZywgMjAwLXIpLCB0K3IsIHQrZywgdCtiXG5cdCAqICAqIOaal+W6puWkhOeQhumAu+i+kSB0PW1heChnLTYwLCByLTEwMCwgYi0xNTApLCByLXQsIGctdCwgYi10XG5cdCAqIOW6n+W8g3JldHVybiBmYWxzZeW5tuS4jeWQiOeQhlxuIyMjXG5saWdodHJnYj0oe3JnYixpc2JhY2s9dHJ1ZX0pIC0+XG5cdHJldHVybiBmYWxzZSBpZihyZ2Jvayh7cmdiLGlzYmFja30pKVxuXHR7cixnLGJ9PXJnYlxuXHRpZiAoaXNiYWNrKSAgI+WkhOeQhuaal+m7keiDjOaZr1xuXHRcdHQ9IE1hdGgubWF4KGctNjAsci0xMDAsIGItMTUwKVxuXHRcdHItPXRcblx0XHRnLT10XG5cdFx0Yi09dFxuXHRlbHNlICAj5aSE55CG5piO5Lqu5YmN5pmvXG5cdFx0dD0gTWF0aC5tYXgoMTAwLWcsMjAwLXIpXG5cdFx0cis9dFxuXHRcdGcrPXRcblx0XHRiKz10XG5cdHtyLGcsYn1cblxuXG5cblxuXG4jIyMqXG4gKiAtLS0tLS0tLS0tLS0tLeWvueavlOW6pumAu+i+kSwg6YWN5ZCIZGFya3N0eWxlLS0tLS0tLS0tLS0tLS0tLS1cbiAqIOi/memHjOeahOiuoeeul+acquW/heWQiOmAgi5cbiMjI1xuXG5cblxuXG5cdFx0IyMjKlxuXHRcdCAqIOS7jnN0cuWIsOS6ruW6plxuXHRcdCAjIyNcbmV4cG9ydCBnZXRsaWdodD0gKHN0cikgLT5cblx0Z2V0cmdibGlnaHQgc3RyMnJnYiBzdHJcblx0XHRcbmV4cG9ydCBnZXRkYXJrPShzdHIpIC0+XG5cdGdldHJnYmRhcmsgc3RyMnJnYiBzdHJcblx0XHRcblxuXG5cbiMjIypcblx0KiDmi7/liLDop4bop4nkuq7luqZcblx0KiDnlKjmnaXliKTmlq3popzoibLmmK/lkKblpJ/kuq5cblx0KiDmnKrmnaXnlKjojaflhYnkvZPlj5HlhYnog73ph4/mnaXorqHnrpfmlbDlgLwuXG4jIyNcbmdldHJnYmxpZ2h0PSAoe3IsZyxifSktPlxuXHRyPXIqMS4yXG5cdGc9ZyoxLjdcblx0bWxpZ2h0KHtyLGcsYn0pLmxpZ2h0XG5cbiMjIypcblx0KiDmi7/liLDop4bop4nmmpfluqZcblx0KiDnlKjmnaXliKTmlq3popzoibLmmK/lkKblpJ/mmpdcblx0KlxuIyMjXG5nZXRyZ2JkYXJrPSh7cixnLGJ9KS0+XG5cdHI9cioxLjNcblx0Zz1nKjEuMlxuXHRtbGlnaHQoe3IsZyxifSkubGlnaHRcblxubWxpZ2h0PSh7cixnLGJ9KS0+XG5cdGlmIHIgPiBnXG5cdFx0YjE9clxuXHRcdGIyPWdcblx0ZWxzZVxuXHRcdGIxPWdcblx0XHRiMj1yXG5cdFxuXHRpZiBiIDwgYjJcblx0XHRiMz1iXG5cdGVsc2UgaWYgYiA8IGIxXG5cdFx0YjM9YjJcblx0XHRiMj1iXG5cdGVsc2Vcblx0XHRiMz1iMlxuXHRcdGIyPWIxXG5cdFx0YjE9YlxuXHRcdFx0IyDov5nph4zlvojkuI3kuKXosKgsIOS6ruW6pueahOW/g+eQhumYiOmZkOa1i+mHj+aIluiuuOiDveiuqei/meS4quiuoeeul+abtOeyvuehri5cblx0bGlnaHQ6IChiMStiMi82KSoyLzMsbWF4OiBbYjEsYjIsYjNdXG5cdFxuXG5cblxuXG5cblxuXG5cbiMjI1xuXHQqIOWfuuehgOW3peWFt+WHveaVsC0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS1cblx0KiDliKTmlq3kuKTkuKpyZ2JzdHLlrp7pmYXnm7jnrYlcbiMjI1xuXG5leHBvcnQgc2FtZXJnYnN0cj0ocmdic3RyMSxyZ2JzdHIyKS0+XG5cdHtyOiByMSxnOiBnMSxiOiBiMX09c3RyMnJnYihyZ2JzdHIxKVxuXHR7cjogcjIsZzogZzIsYjogYjJ9PXN0cjJyZ2Iocmdic3RyMilcblx0cjE9PXIyJiZnMT09ZzImJmIxPT1iMlxuXG5cbiMjIypcbiogM+enjeaDheWGtVxuKiAxLiByZ2Lmg4XlhrU6IOS7jlwicmdiKDE1NCwgMTU0LCAwKVwi6L2s5YyW5Li6OiB7cjoxNTQsZzoxNTQsYjowLH1cbiogXCJyZ2JhKDAsIDAsIDAsIDApXCIg6L+Y6ZyA5YW85a655q2k56eN5oOF5Ya1XG4qIDIuIDE26L+b5Yi25oOF5Ya1OiAgI2ZjMiAgI2ZjMjkgI2ZjMjkyOTIzICNmYzI5MjlcbiogMy4g6aKc6Imy5ZCNOiB3aGl0ZSwgYmxhY2ssIG9yZ2FuZ2VcbiojIyNcblxuc3RyMnJnYj0oc3RyKS0+XG5cblx0I1wicmdiKDE1NCwgMTU0LCAwKVwiXG5cdHJlZz0vcmdiYT9cXCgoWzAtOV17MSwzfSksID8oWzAtOV17MSwzfSksID8oWzAtOV17MSwzfSkoLCA/KFswLTkuJV0rKSk/XFwpL2lcblx0Yz1yZWcuZXhlYyBzdHJcblxuXHQjMTbov5vliLbpopzoibJcblx0Yz89aGV4MnJnYihzdHIpXG5cblx0I+WGjeWIpOaWreWQjeWtl+minOiJsjpcblx0Yz89IGNvbG9ybmFtZVtzdHJdICYmIGhleDJyZ2IoaGV4KVxuXHRcblx0cmV0dXJuIGZhbHNlIHVubGVzcyBjP1xuXG5cblx0I+mAj+aYjuW6plxuXHRhPSBpZiBjWzVdPyB0aGVuIGNbNV0gZWxzZSAxXG5cdHtyOiArY1sxXSxnOiArY1syXSxiOiArY1szXSxhfSAj6L+Z6YeM55SoK+aYr+S4uuS6huaKiuWtl+espuS4sui9rOS4uuaVsOWtlywg5LiN54S25Lya5Y+R55Sf5b6I6ZqQ6JS955qEYnVnLlxuXG5cblx0IyMjXG5cdCAqIOS7jnJnYuWIsHN0clxuXHQgKiMjI1xucmdiMnN0cj0oe3IsZyxiLGE9MX0pIC0+XG5cdGlmIGE+MC45OSB0aGVuICBcInJnYihcIityK1wiLFwiK2crXCIsXCIrYitcIilcIiBlbHNlIFwicmdiYShcIityK1wiLFwiK2crXCIsXCIrYitcIixcIithK1wiKVwiXG5cblxuXHQjIyNcblx0ICog5LuOMTbov5vliLbliLByZ2Jcblx0ICogICNmYzIgICNmYzI5ICNmYzI5MjkyMyAjZmMyOTI5LCAzLDQsNiw46YO95pyJ5Y+v6IO9XG5cdCMjI1xuXG5cbmhleDJyZ2I9KHN0cikgLT5cblx0cmVnPS8jKFteI10qKS9pICPliKTmlq0jNjJmYTg56L+Z56eN57G75Ly85pa55byP55qE6aKc6Imy5YC8XG5cdGM9cmVnLmV4ZWMoc3RyKVxuXHRyZXR1cm4gZmFsc2UgdW5sZXNzIGM/WzFdPyBcdCNjMeS4jeWtmOWcqOmcgOimgXJldHVyblxuXHR4PWNbMV1cblx0eT1bXVxuXHRzd2l0Y2ggeC5sZW5ndGhcblx0XHR3aGVuIDNcblx0XHRcdHlbMV09cGFyc2VJbnQoJycreFswXSt4WzBdLDE2KVxuXHRcdFx0eVsyXT1wYXJzZUludCgnJyt4WzFdK3hbMV0sMTYpXG5cdFx0XHR5WzNdPXBhcnNlSW50KCcnK3hbMl0reFsyXSwxNilcblx0XHR3aGVuIDRcblx0XHRcdCN5WzFdPXBhcnNlSW50KHhbMF0sMTYpOyAgLzIwMjAxMDI45L+u5q2j6ZSZ6K+vLCDov5nkuKrkvJrmiopmZmbovazljJbkuLogcjE1ZzE1YjE1LCDlhbblrp7lupTor6XmmK9yMjU1ZzI1NWIyNTVcblx0XHRcdHlbMV09cGFyc2VJbnQoJycreFswXSt4WzBdLDE2KVxuXHRcdFx0eVsyXT1wYXJzZUludCgnJyt4WzFdK3hbMV0sMTYpXG5cdFx0XHR5WzNdPXBhcnNlSW50KCcnK3hbMl0reFsyXSwxNilcblx0XHRcdHlbNV09cGFyc2VJbnQoeFszXSwxNikvMjU1XG5cdFx0d2hlbiA2XG5cdFx0XHR5WzFdPXBhcnNlSW50KCcnK3hbMF0reFsxXSwxNilcblx0XHRcdHlbMl09cGFyc2VJbnQoJycreFsyXSt4WzNdLDE2KVxuXHRcdFx0eVszXT1wYXJzZUludCgnJyt4WzRdK3hbNV0sMTYpXG5cdFx0d2hlbiA4XG5cdFx0XHR5WzFdPXBhcnNlSW50KCcnK3hbMF0reFsxXSwxNilcblx0XHRcdHlbMl09cGFyc2VJbnQoJycreFsyXSt4WzNdLDE2KVxuXHRcdFx0eVszXT1wYXJzZUludCgnJyt4WzRdK3hbNV0sMTYpXG5cdFx0XHR5WzVdPXBhcnNlSW50KCcnK3hbNl0reFs3XSwxNikvMjU1XG5cdFx0I+S4jea7oei2s+S4iumdouWbm+S4quaDheWGtSwg55u05o6l6L+U5ZueZmFsc2Vcblx0XHRlbHNlIHJldHVybiBmYWxzZVxuXHR5XG5cblx0I2NvbnNvbGUubG9nKHN0cjJyZ2IoJ3doaXRlJykpOyAjdGVzdFxuXG5cblxuXG5cblxudGVzdCA9IC0+XG5cdHJlc3VsdD13aW5kb3cubHVvY2xyLmxpZ2h0c3RyIHN0cjond2hpdGUnLCBpc2JhY2s6dHJ1ZVxuXHRjb25zb2xlLmxvZyB7cmVzdWx0fVxuXG5cbmV4cG9ydCBkZWZhdWx0IHtcblx0I2xpZ2h0c3RyXG5cdCNsaWdodHJnYlxuXHRsc3RyXG5cdGxyZ2Jcblx0Z2V0bGlnaHRcblx0Z2V0ZGFya1xuXHRzYW1lcmdic3RyXG5cbn1cbiIsIiNpbXBvcnQge2xvZ30gZnJvbSAnLi9sb2cuanMnXG5cbmltcG9ydCBmcyBmcm9tICdmcy9wcm9taXNlcydcbmltcG9ydCBmdHMgZnJvbSAnZnMnXG5cbmltcG9ydCBvcyBmcm9tICdvcydcblxuaG9tZSA9IG9zLmhvbWVkaXIoKVxubG9nPWNvbnNvbGUubG9nXG5cblxuIyB0b2RvIOS/neWtmOaWh+S7tlxuZXhwb3J0IHNhdmVmaWxlPSh7anNvbixmaWxlbmFtZT1cInRlbXB0ZXN0LnR4dFwifSktPlxuXHRmcy53cml0ZUZpbGUgZmlsZW5hbWUsSlNPTi5zdHJpbmdpZnkoanNvbiksKGVyciktPlxuXHRcdGxvZyAn5paH5Lu25YaZ5YWlJyxmaWxlbmFtZVxuXHRcdGxvZyBlcnIgaWYgZXJyXG5cbiMg5ou/5Yiw55yf5q2j55qEcGF0aFxuZXhwb3J0IHJlYWxwYXRoPShwYXRoKS0+XG5cdCMgaG9tZeaWh+S7tuWkuX4sIG5vZGXkuI3orqQsIOimgeiwg+aVtOS4um9zLmhvbWVkaXJcblx0aWYgL15+Ly50ZXN0IHBhdGggdGhlbiBwYXRoLnJlcGxhY2UgL15+LywgaG9tZSAgZWxzZSBwYXRoXG5cblxuIyDliqDmlofku7blhoXlrrksIOimhuebluW8j1xuZXhwb3J0IGNvdmVyPSh7cGF0aCwgc3RyLCBvcHRpb25zPWVuY29kaW5nOid1dGY4J30pIC0+XG5cblx0IyDopobnm5blvI/lhpnlhaVcblx0YXdhaXQgZnMud3JpdGVGaWxlIChyZWFscGF0aCBwYXRoKSwgc3RyLCBvcHRpb25zXG5cdHRydWVcbiMg6L+95Yqg5paH5Lu25YaF5a65LCDkv53or4Hov73liqDlhoXlrrnkuI3ph43lpI1cbmV4cG9ydCB1bmlxdWVhZGQ9KHtwYXRoLCByZWcsIHN0ciwgb3B0aW9ucz1lbmNvZGluZzondXRmOCd9KSAtPlxuXG5cdCMgaG9tZeaWh+S7tuWkuX4sIG5vZGXkuI3orqQsIOimgeiwg+aVtOS4um9zLmhvbWVkaXJcblx0cnBhdGggPXJlYWxwYXRoIHBhdGhcblxuXHQjIOivu+WPluaWh+S7tlxuXHRmc3RyPWF3YWl0IGZzLnJlYWRGaWxlIHJwYXRoLCBvcHRpb25zXG5cdCMg5q2j5YiZ5Yik5patXG5cdHJldHVybiBmYWxzZSBpZiByZWcudGVzdCBmc3RyXG5cblx0IyDkuI3ph43lpI3lsLHmj5LlhaVcblx0YXdhaXQgZnMuYXBwZW5kRmlsZSBycGF0aCwgc3RyID8gcmVnLnNvdXJjZSwgb3B0aW9uc1xuXHR0cnVlXG5cbiMg5Yik5pat5piv5paH5Lu2L+ebruW9lVxuZXhwb3J0IGZpbGV0eXBlID0gKHBhdGgpLT5cblx0c3RhdD1hd2FpdCBmcy5sc3RhdCByZWFscGF0aCBwYXRoXG5cdHJldHVybiBpc2ZpbGU6IHN0YXQuaXNGaWxlKCksIGlzZGlyOiBzdGF0LmlzRGlyZWN0b3J5KClcblxuIyDliKTmlq3mlofku7Yv55uu5b2V5piv5ZCm5a2Y5ZyoXG5leHBvcnQgZXhpc3QgPSAocGF0aCktPlxuXHRmdHMuZXhpc3RzU3luYyByZWFscGF0aCBwYXRoXG5cbiMg5bu65LiA5Liq55uu5b2VXG5leHBvcnQgbWtkaXI9KHBhdGgpLT5cblx0YXdhaXQgZnMubWtkaXIgKHJlYWxwYXRoIHBhdGgpLHJlY3Vyc2l2ZTp0cnVlXG5cbiMg5aaC5p6c5LiN5a2Y5Zyo5bCx5bu65LiA5Liq55uu5b2VXG5leHBvcnQgbmV3ZGlyPShwYXRoKS0+XG5cdHJldHVybiBmYWxzZSBpZiBleGlzdCBwYXRoXG5cdGF3YWl0IGZzLm1rZGlyIHJlYWxwYXRoIHBhdGhcblxuIyDor7vlj5bnm67lvZVcbmV4cG9ydCByZWFkaXIgPSAocGF0aCktPlxuXHRhd2FpdCBmcy5yZWFkZGlyIHJlYWxwYXRoIHBhdGhcbiMg6K+75Y+W5paH5Lu2XG5leHBvcnQgcmVhZGZpbGUgPSAocGF0aCktPlxuXHRmc3RyPWF3YWl0IGZzLnJlYWRGaWxlIChyZWFscGF0aCBwYXRoKSwgZW5jb2Rpbmc6J3V0ZjgnXG5cblxuIyB0b2RvIOivu+WPluebruW9leS4reaJgOaciemXrumimCwg5rex5bqm6YGN5Y6GXG5leHBvcnQgYWxsZmlsZSA9ICh7cGF0aCwgcmVnPWZhbHNlLG9wdGlvbnM9ZW5jb2Rpbmc6J3V0ZjgnfSktPlxuXHQjIOWmguaenOayoeS8oHJlZywg6YKj5LmI5bCx5piv6KaB5omA5pyJ5paH5Lu2XG5cdHJlYWRkaXIocGF0aCwgb3B0aW9ucylcblxuXG5cblxuZXhwb3J0IGRlZmF1bHQge1xuXHRzYXZlZmlsZVxuXHRjb3ZlclxuXHR1bmlxdWVhZGRcblx0ZmlsZXR5cGVcblx0ZXhpc3Rcblx0bWtkaXJcblx0cmVhZGlyXG5cdHJlYWRmaWxlXG5cdGFsbGZpbGVcbn1cbiIsImV4cG9ydCB7ZGVmYXVsdCBhcyBjb2xvcn0gZnJvbSAnLi9jb2xvci5jcydcbmV4cG9ydCB7ZGVmYXVsdCBhcyByYW5kb219IGZyb20gXHQnLi9yYW5kb20uY3MnXG5leHBvcnQge2RlZmF1bHQgYXMgdGltZX0gZnJvbSBcdCcuL3RpbWUuY3MnXG5leHBvcnQge2RlZmF1bHQgYXMgZmlsZX0gZnJvbSBcdCcuL2ZpbGUuY3MnXG4iLCJcbiMjI1xuKiDmi7/liLDmlbTmlbDniYjmnKznmoTpmo/mnLrmlbBcbiog5Zug5Li6cmFuZG9t5LiN5YyF5ZCrMSwg5Zug5q2kLCDov5nkuKrpmo/mnLrmlbDkuZ/kuI3ljIXlkKttYXhcbiog5Zug5q2k5Lyg5YWl5pWw57uE55qE6ZW/5bqm5q2j5aW95ZCI6YCC5L2c5Li65pWw57uE55qE57Si5byVXG4jIyNcbmltcG9ydCB7bHN0cn0gZnJvbSAnLi9jb2xvci5jcydcblxuZXhwb3J0IHJhbmRvbWludD0obWF4KSAtPlxuXHQoTWF0aC5yYW5kb20oKSptYXgpPj4wXG5cbiMjI1xu5pWw57uE5Lit5Y+W5LiA5Liq6ZqP5py6ZWxlbWVudFxuIyMjXG5leHBvcnQgcmFuZG9tZW1iZXIgPSAocCkgLT5cblx0cFtyYW5kb21pbnQgcC5sZW5ndGhdXG5cbiMg6ZqP5py65LiA5Liq6aKc6ImyXG5leHBvcnQgcmFuZG9tY29sb3IgPSAtPlxuXHRcInJnYihcIisgKHJhbmRvbWludCAyNTYpK1wiLFwiKyhyYW5kb21pbnQgMjU2ICkrXCIsXCIrKHJhbmRvbWludCAyNTYpK1wiKVwiXG5cblxuIyDpmo/mnLrkuIDkuKrmmI7kuq7popzoibJcbmV4cG9ydCByYW5kb21saWdodCA9IC0+XG5cdGxzdHJcblx0XHRzdHI6cmFuZG9tY29sb3IgMFxuXHRcdGlzYmFjazpmYWxzZVxuXG5cblxuXG5leHBvcnQgZGVmYXVsdCB7XG5cdHJhbmRvbWludFxuXHRyYW5kb21lbWJlclxuXHRyYW5kb21jb2xvclxuXHRyYW5kb21saWdodFxufVxuIiwiZXhwb3J0IHNsZWVwID0gKG1zKSAtPlxuXHRuZXcgUHJvbWlzZSAocmVzb2x2ZSkgLT5cblx0XHRzZXRUaW1lb3V0IHJlc29sdmUsIG1zXG5cblxuZXhwb3J0IGdldGhobW0gPSh0aW1lc3RhbXA9RGF0ZS5ub3coKSktPlxuXHR0PW5ldyBEYXRlKCt0aW1lc3RhbXApXG5cdHQuZ2V0SG91cnMoKSsnOicrdC5nZXRNaW51dGVzKClcblxuZXhwb3J0IGdldGhobW1zcyA9KHRpbWVzdGFtcD1EYXRlLm5vdygpKS0+XG5cdHQ9bmV3IERhdGUoK3RpbWVzdGFtcClcblx0dC5nZXRIb3VycygpKyc6Jyt0LmdldE1pbnV0ZXMoKSsnOicrdC5nZXRTZWNvbmRzKClcblxuXG5cblxuZXhwb3J0IGRlZmF1bHQge1xuXHRzbGVlcFxuXHRnZXRoaG1tXG5cdGdldGhobW1zc1xufVxuIiwibW9kdWxlLmV4cG9ydHMgPSB7XCJibGFja1wiOlwiIzAwMFwiLFwic2lsdmVyXCI6XCIjYzBjMGMwXCIsXCJncmF5XCI6XCIjODA4MDgwXCIsXCJ3aGl0ZVwiOlwiI2ZmZlwiLFwibWFyb29uXCI6XCIjODAwMDAwXCIsXCJyZWRcIjpcIiNmZjAwMDBcIixcInB1cnBsZVwiOlwiIzgwMDA4MFwiLFwiZnVjaHNpYVwiOlwiI2ZmMDBmZlwiLFwiZ3JlZW5cIjpcIiMwMDgwMDBcIixcImxpbWVcIjpcIiMwMGZmMDBcIixcIm9saXZlXCI6XCIjODA4MDAwXCIsXCJ5ZWxsb3dcIjpcIiNmZmZmMDBcIixcIm5hdnlcIjpcIiMwMDAwODBcIixcImJsdWVcIjpcIiMwMDAwZmZcIixcInRlYWxcIjpcIiMwMDgwODBcIixcImFxdWFcIjpcIiMwMGZmZmZcIixcIm9yYW5nZVwiOlwiI2ZmYTUwMFwiLFwiYWxpY2VibHVlXCI6XCIjZjBmOGZmXCIsXCJhbnRpcXVld2hpdGVcIjpcIiNmYWViZDdcIixcImFxdWFtYXJpbmVcIjpcIiM3ZmZmZDRcIixcImF6dXJlXCI6XCIjZjBmZmZmXCIsXCJiZWlnZVwiOlwiI2Y1ZjVkY1wiLFwiYmlzcXVlXCI6XCIjZmZlNGM0XCIsXCJibGFuY2hlZGFsbW9uZFwiOlwiI2ZmZWJjZFwiLFwiYmx1ZXZpb2xldFwiOlwiIzhhMmJlMlwiLFwiYnJvd25cIjpcIiNhNTJhMmFcIixcImJ1cmx5d29vZFwiOlwiI2RlYjg4N1wiLFwiY2FkZXRibHVlXCI6XCIjNWY5ZWEwXCIsXCJjaGFydHJldXNlXCI6XCIjN2ZmZjAwXCIsXCJjaG9jb2xhdGVcIjpcIiNkMjY5MWVcIixcImNvcmFsXCI6XCIjZmY3ZjUwXCIsXCJjb3JuZmxvd2VyYmx1ZVwiOlwiIzY0OTVlZFwiLFwiY29ybnNpbGtcIjpcIiNmZmY4ZGNcIixcImNyaW1zb25cIjpcIiNkYzE0M2NcIixcImN5YW5cIjpcIiMwMGZmZmZcIixcImRhcmtibHVlXCI6XCIjMDAwMDhiXCIsXCJkYXJrY3lhblwiOlwiIzAwOGI4YlwiLFwiZGFya2dvbGRlbnJvZFwiOlwiI2I4ODYwYlwiLFwiZGFya2dyYXlcIjpcIiNhOWE5YTlcIixcImRhcmtncmVlblwiOlwiIzAwNjQwMFwiLFwiZGFya2dyZXlcIjpcIiNhOWE5YTlcIixcImRhcmtraGFraVwiOlwiI2JkYjc2YlwiLFwiZGFya21hZ2VudGFcIjpcIiM4YjAwOGJcIixcImRhcmtvbGl2ZWdyZWVuXCI6XCIjNTU2YjJmXCIsXCJkYXJrb3JhbmdlXCI6XCIjZmY4YzAwXCIsXCJkYXJrb3JjaGlkXCI6XCIjOTkzMmNjXCIsXCJkYXJrcmVkXCI6XCIjOGIwMDAwXCIsXCJkYXJrc2FsbW9uXCI6XCIjZTk5NjdhXCIsXCJkYXJrc2VhZ3JlZW5cIjpcIiM4ZmJjOGZcIixcImRhcmtzbGF0ZWJsdWVcIjpcIiM0ODNkOGJcIixcImRhcmtzbGF0ZWdyYXlcIjpcIiMyZjRmNGZcIixcImRhcmtzbGF0ZWdyZXlcIjpcIiMyZjRmNGZcIixcImRhcmt0dXJxdW9pc2VcIjpcIiMwMGNlZDFcIixcImRhcmt2aW9sZXRcIjpcIiM5NDAwZDNcIixcImRlZXBwaW5rXCI6XCIjZmYxNDkzXCIsXCJkZWVwc2t5Ymx1ZVwiOlwiIzAwYmZmZlwiLFwiZGltZ3JheVwiOlwiIzY5Njk2OVwiLFwiZGltZ3JleVwiOlwiIzY5Njk2OVwiLFwiZG9kZ2VyYmx1ZVwiOlwiIzFlOTBmZlwiLFwiZmlyZWJyaWNrXCI6XCIjYjIyMjIyXCIsXCJmbG9yYWx3aGl0ZVwiOlwiI2ZmZmFmMFwiLFwiZm9yZXN0Z3JlZW5cIjpcIiMyMjhiMjJcIixcImdhaW5zYm9yb1wiOlwiI2RjZGNkY1wiLFwiZ2hvc3R3aGl0ZVwiOlwiI2Y4ZjhmZlwiLFwiZ29sZFwiOlwiI2ZmZDcwMFwiLFwiZ29sZGVucm9kXCI6XCIjZGFhNTIwXCIsXCJncmVlbnllbGxvd1wiOlwiI2FkZmYyZlwiLFwiZ3JleVwiOlwiIzgwODA4MFwiLFwiaG9uZXlkZXdcIjpcIiNmMGZmZjBcIixcImhvdHBpbmtcIjpcIiNmZjY5YjRcIixcImluZGlhbnJlZFwiOlwiI2NkNWM1Y1wiLFwiaW5kaWdvXCI6XCIjNGIwMDgyXCIsXCJpdm9yeVwiOlwiI2ZmZmZmMFwiLFwia2hha2lcIjpcIiNmMGU2OGNcIixcImxhdmVuZGVyXCI6XCIjZTZlNmZhXCIsXCJsYXZlbmRlcmJsdXNoXCI6XCIjZmZmMGY1XCIsXCJsYXduZ3JlZW5cIjpcIiM3Y2ZjMDBcIixcImxlbW9uY2hpZmZvblwiOlwiI2ZmZmFjZFwiLFwibGlnaHRibHVlXCI6XCIjYWRkOGU2XCIsXCJsaWdodGNvcmFsXCI6XCIjZjA4MDgwXCIsXCJsaWdodGN5YW5cIjpcIiNlMGZmZmZcIixcImxpZ2h0Z29sZGVucm9keWVsbG93XCI6XCIjZmFmYWQyXCIsXCJsaWdodGdyYXlcIjpcIiNkM2QzZDNcIixcImxpZ2h0Z3JlZW5cIjpcIiM5MGVlOTBcIixcImxpZ2h0Z3JleVwiOlwiI2QzZDNkM1wiLFwibGlnaHRwaW5rXCI6XCIjZmZiNmMxXCIsXCJsaWdodHNhbG1vblwiOlwiI2ZmYTA3YVwiLFwibGlnaHRzZWFncmVlblwiOlwiIzIwYjJhYVwiLFwibGlnaHRza3libHVlXCI6XCIjODdjZWZhXCIsXCJsaWdodHNsYXRlZ3JheVwiOlwiIzc3ODg5OVwiLFwibGlnaHRzbGF0ZWdyZXlcIjpcIiM3Nzg4OTlcIixcImxpZ2h0c3RlZWxibHVlXCI6XCIjYjBjNGRlXCIsXCJsaWdodHllbGxvd1wiOlwiI2ZmZmZlMFwiLFwibGltZWdyZWVuXCI6XCIjMzJjZDMyXCIsXCJsaW5lblwiOlwiI2ZhZjBlNlwiLFwibWFnZW50YVwiOlwiI2ZmMDBmZlwiLFwibWVkaXVtYXF1YW1hcmluZVwiOlwiIzY2Y2RhYVwiLFwibWVkaXVtYmx1ZVwiOlwiIzAwMDBjZFwiLFwibWVkaXVtb3JjaGlkXCI6XCIjYmE1NWQzXCIsXCJtZWRpdW1wdXJwbGVcIjpcIiM5MzcwZGJcIixcIm1lZGl1bXNlYWdyZWVuXCI6XCIjM2NiMzcxXCIsXCJtZWRpdW1zbGF0ZWJsdWVcIjpcIiM3YjY4ZWVcIixcIm1lZGl1bXNwcmluZ2dyZWVuXCI6XCIjMDBmYTlhXCIsXCJtZWRpdW10dXJxdW9pc2VcIjpcIiM0OGQxY2NcIixcIm1lZGl1bXZpb2xldHJlZFwiOlwiI2M3MTU4NVwiLFwibWlkbmlnaHRibHVlXCI6XCIjMTkxOTcwXCIsXCJtaW50Y3JlYW1cIjpcIiNmNWZmZmFcIixcIm1pc3R5cm9zZVwiOlwiI2ZmZTRlMVwiLFwibW9jY2FzaW5cIjpcIiNmZmU0YjVcIixcIm5hdmFqb3doaXRlXCI6XCIjZmZkZWFkXCIsXCJvbGRsYWNlXCI6XCIjZmRmNWU2XCIsXCJvbGl2ZWRyYWJcIjpcIiM2YjhlMjNcIixcIm9yYW5nZXJlZFwiOlwiI2ZmNDUwMFwiLFwib3JjaGlkXCI6XCIjZGE3MGQ2XCIsXCJwYWxlZ29sZGVucm9kXCI6XCIjZWVlOGFhXCIsXCJwYWxlZ3JlZW5cIjpcIiM5OGZiOThcIixcInBhbGV0dXJxdW9pc2VcIjpcIiNhZmVlZWVcIixcInBhbGV2aW9sZXRyZWRcIjpcIiNkYjcwOTNcIixcInBhcGF5YXdoaXBcIjpcIiNmZmVmZDVcIixcInBlYWNocHVmZlwiOlwiI2ZmZGFiOVwiLFwicGVydVwiOlwiI2NkODUzZlwiLFwicGlua1wiOlwiI2ZmYzBjYlwiLFwicGx1bVwiOlwiI2RkYTBkZFwiLFwicG93ZGVyYmx1ZVwiOlwiI2IwZTBlNlwiLFwicm9zeWJyb3duXCI6XCIjYmM4ZjhmXCIsXCJyb3lhbGJsdWVcIjpcIiM0MTY5ZTFcIixcInNhZGRsZWJyb3duXCI6XCIjOGI0NTEzXCIsXCJzYWxtb25cIjpcIiNmYTgwNzJcIixcInNhbmR5YnJvd25cIjpcIiNmNGE0NjBcIixcInNlYWdyZWVuXCI6XCIjMmU4YjU3XCIsXCJzZWFzaGVsbFwiOlwiI2ZmZjVlZVwiLFwic2llbm5hXCI6XCIjYTA1MjJkXCIsXCJza3libHVlXCI6XCIjODdjZWViXCIsXCJzbGF0ZWJsdWVcIjpcIiM2YTVhY2RcIixcInNsYXRlZ3JheVwiOlwiIzcwODA5MFwiLFwic2xhdGVncmV5XCI6XCIjNzA4MDkwXCIsXCJzbm93XCI6XCIjZmZmYWZhXCIsXCJzcHJpbmdncmVlblwiOlwiIzAwZmY3ZlwiLFwic3RlZWxibHVlXCI6XCIjNDY4MmI0XCIsXCJ0YW5cIjpcIiNkMmI0OGNcIixcInRoaXN0bGVcIjpcIiNkOGJmZDhcIixcInRvbWF0b1wiOlwiI2ZmNjM0N1wiLFwidHJhbnNwYXJlbnRcIjpcIiMwMDAwXCIsXCJ0dXJxdW9pc2VcIjpcIiM0MGUwZDBcIixcInZpb2xldFwiOlwiI2VlODJlZVwiLFwid2hlYXRcIjpcIiNmNWRlYjNcIixcIndoaXRlc21va2VcIjpcIiNmNWY1ZjVcIixcInllbGxvd2dyZWVuXCI6XCIjOWFjZDMyXCIsXCJyZWJlY2NhcHVycGxlXCI6XCIjNjYzMzk5XCJ9IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy9hc3luY1RvR2VuZXJhdG9yXCIpOzsiLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJAYmFiZWwvcnVudGltZS9oZWxwZXJzL2ludGVyb3BSZXF1aXJlRGVmYXVsdFwiKTs7IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiQGJhYmVsL3J1bnRpbWUvcmVnZW5lcmF0b3JcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcImNoYWxrXCIpOzsiLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJmc1wiKTs7IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiZnMvcHJvbWlzZXNcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcIm9zXCIpOzsiXSwic291cmNlUm9vdCI6IiJ9