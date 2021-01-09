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
exports.id = "lornpug";
exports.ids = ["lornpug"];
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
//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly9tbGliL3dlYnBhY2svdW5pdmVyc2FsTW9kdWxlRGVmaW5pdGlvbiIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL2xvcm5wdWcuY3MiLCJ3ZWJwYWNrOi8vbWxpYi8uL3NyYy9sb3JucHVnaGVscC5jcyIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL21saWIvc3JjL2NvbG9yLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvZmlsZS5jcyIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL21saWIvc3JjL2luZGV4LmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvcmFuZG9tLmNzIiwid2VicGFjazovL21saWIvLi9zcmMvbWxpYi9zcmMvdGltZS5jcyIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL21saWIvc3JjL2NvbG9ybmFtZS5jc29uIiwid2VicGFjazovL21saWIvZXh0ZXJuYWwgXCJAYmFiZWwvcnVudGltZS9oZWxwZXJzL2FzeW5jVG9HZW5lcmF0b3JcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy9pbnRlcm9wUmVxdWlyZURlZmF1bHRcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiQGJhYmVsL3J1bnRpbWUvcmVnZW5lcmF0b3JcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiY2hhbGtcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiZnNcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiZnMvcHJvbWlzZXNcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwib3NcIiJdLCJuYW1lcyI6W10sIm1hcHBpbmdzIjoiO0FBQUE7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0EsQ0FBQztBQUNELE87Ozs7Ozs7Ozs7Ozs7Ozs7O0FDVkE7O0FBR0E7O0FBSEE7QUFLQSxLLEdBTEEsdUIsQ0FLQSxLO0FBQUEsSSxHQUxBLHVC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QUFELFk7QUFGRixDQUFoQjs7OztBQUlBLElBQU8sWUFBVyxTQUFYLFNBQVc7QUFBQSxNQUFDLFNBQUQsdUVBQVcsSUFBSSxDQUFmLEdBQVcsRUFBWDtBQUNsQjtBQUFDLE1BQUUsU0FBUyxDQUFULFVBQUY7U0FDQSxDQUFDLENBQUQsbUJBQWlCLENBQUMsQ0FBbEIsVUFBaUIsRUFBakIsU0FBb0MsQ0FBQyxDQUFELFk7QUFGbkIsQ0FBbEI7OztlQU9lO0FBQ2QsT0FEYyxFQUNkLEtBRGM7QUFFZCxTQUZjLEVBRWQsT0FGYztBQUdkLFdBSGMsRUFHZDtBQUhjLEM7Ozs7Ozs7Ozs7O0FDaEJmLGtCQUFrQiw4ckc7Ozs7Ozs7Ozs7O0FDQWxCLHFFOzs7Ozs7Ozs7OztBQ0FBLDBFOzs7Ozs7Ozs7OztBQ0FBLHdEOzs7Ozs7Ozs7OztBQ0FBLG1DOzs7Ozs7Ozs7OztBQ0FBLGdDOzs7Ozs7Ozs7OztBQ0FBLHlDOzs7Ozs7Ozs7OztBQ0FBLGdDIiwiZmlsZSI6Imxvcm5wdWcvbG9ybnB1Zy5qcyIsInNvdXJjZXNDb250ZW50IjpbIihmdW5jdGlvbiB3ZWJwYWNrVW5pdmVyc2FsTW9kdWxlRGVmaW5pdGlvbihyb290LCBmYWN0b3J5KSB7XG5cdGlmKHR5cGVvZiBleHBvcnRzID09PSAnb2JqZWN0JyAmJiB0eXBlb2YgbW9kdWxlID09PSAnb2JqZWN0Jylcblx0XHRtb2R1bGUuZXhwb3J0cyA9IGZhY3RvcnkoKTtcblx0ZWxzZSBpZih0eXBlb2YgZGVmaW5lID09PSAnZnVuY3Rpb24nICYmIGRlZmluZS5hbWQpXG5cdFx0ZGVmaW5lKFtdLCBmYWN0b3J5KTtcblx0ZWxzZSBpZih0eXBlb2YgZXhwb3J0cyA9PT0gJ29iamVjdCcpXG5cdFx0ZXhwb3J0c1tcIm1saWJcIl0gPSBmYWN0b3J5KCk7XG5cdGVsc2Vcblx0XHRyb290W1wibWxpYlwiXSA9IGZhY3RvcnkoKTtcbn0pKGdsb2JhbCwgZnVuY3Rpb24oKSB7XG5yZXR1cm4gIiwiaW1wb3J0IGNoYWxrIGZyb20gJ2NoYWxrJ1xuI2NoYWxrID0gcmVxdWlyZSgnY2hhbGsnKVxuI2ltcG9ydCB7ZmlsZX0gZnJvbSAnbWxpYidcbmltcG9ydCBscGggZnJvbSAnLi9sb3JucHVnaGVscC5jcydcblxue2lzY21kLCBkb2xwLCBkb2Rpcn09bHBoXG5cblxuIyAtLS0tLS0tLS0tLS0tLS0tLS1pbml0LS0tLS0tLS0tLS0tLS0tLS0tLS0tXG5hcmc9cHJvY2Vzcy5hcmd2WzIuLl1cbmNvbnNvbGUubG9nICfnjq/looPlkozlj4LmlbDkv6Hmga86J1xuY29uc29sZS50YWJsZSBbXG5cdHsn6ZSuOiAnOidwcm9jZXNzLmN3ZCgpJywgJ+WAvDogJzogcHJvY2Vzcy5jd2QoKX1cblx0eyfplK46ICc6J19fZGlybmFtZScsICflgLw6ICc6X19kaXJuYW1lfVxuXHR7J+mUrjogJzoncHJvY2Vzcy5hcmd2JywgJ+WAvDogJzpwcm9jZXNzLmFyZ3Z9XG5cdHsn6ZSuOiAnOifnnJ/lj4LmlbAnLCAn5YC8OiAnOmFyZ31cbl1cblxuY21kcz1bXSAjIOS8oOWFpeeahOWPguaVsOeahOWRveS7pOaVsOe7hFxuZmlsZXM9W10gIyDkvKDlhaXnmoTmlofku7bmlbDnu4RcblxuZm9yIHggaW4gYXJnXG5cdGlmIGlzY21kIHggdGhlbiBjbWRzLnB1c2ggeCBlbHNlIGZpbGVzLnB1c2ggeFxuXG5cbmNvbnNvbGUubG9nIGNoYWxrLmJsdWVCcmlnaHQgXCLlkb3ku6Q6ICAje2NtZHN9IFwiXG5jb25zb2xlLmxvZyBjaGFsay55ZWxsb3dCcmlnaHQgXCLmlofku7Y6ICN7ZmlsZXN9IFwiXG5cblxuIyDlpITnkIblkb3ku6RcbmlzZGVmYXVsdCA9IGNtZHMubGVuZ3RoID09MFxuZm9yIHggaW4gY21kc1xuXHRpc2Rpcj0vXi0tZGlyJC8udGVzdCB4XG5cdGlzaW89L14tLWlvJC8udGVzdCB4XG5cdGlzbHA9L14tLWxwJC8udGVzdCB4XG5cblxuXG5cbiMg5qC55o2u5ZG95LukLCDlpITnkIbmlofku7YsIOW/veeVpW5vZGVfbW9kdWxlLCDmnKrmnaXmoLnmja4uZ2l0aWdub3Jl5b+955WlXG5pZiBpc2RlZmF1bHQgIyDpu5jorqTmmK/ot5HmtYvor5Xmlofku7Zcblx0Y29uc29sZS5sb2cgKGNoYWxrLmN5YW5CcmlnaHQgJ+m7mOiupOihjOS4ujonKSwgJ+a1i+ivlee7k+aenDogJ1xuXG5lbHNlIGlmIGlzbHAgIyDkv67mlLkuenNocmPmlofku7YsIOWinuWKoOW/q+aNt+aWueW8j1xuXHRkb2xwIDBcbmVsc2UgaWYgaXNkaXIgIyDlu7rmtYvor5Xnm67lvZUsIOWIneWni+WMlua1i+ivleaWh+S7tlxuXHRpZiBmaWxlcy5sZW5ndGg9PTBcblx0XHRkb2RpciAnLicgI+WmguaenOayoeacieS7u+S9leaWh+S7tuWPguaVsCwg6YKj5LmI5bCx5piv5LuO5pys5Zyw5pCe5rex5bqm6YGN5Y6GXG5cdGVsc2Vcblx0XHRkb2RpciB4IGZvciB4IGluIGZpbGVzXG5cdFx0XG5cdGNvbnNvbGUubG9nIChjaGFsay5jeWFuQnJpZ2h0ICfliJ3lp4vljJYnKSwgJ+WIneWni+WMluaIkOWKnzogJ1xuXG5lbHNlIGlmIGlzaW8gIyDlop7liqDmtYvor5Xmlofku7Zcblx0Y29uc29sZS5sb2cgKGNoYWxrLmN5YW5CcmlnaHQgJ+WIneWni+WMlicpLCAn5Yid5aeL5YyW5oiQ5YqfOiAnXG5cdFxuZWxzZVxuXHRjb25zb2xlLmxvZyBjaGFsay5yZWRCcmlnaHQgJ+aKseatieWVijog5LiN55+l6YGT5L2g5oOz5bmy5ZWlJ1xuXG5cblxuXG4jIyNcbnRyeSB7XG5cdG5ldyBGdW5jdGlvbigndmFyIHthfSA9IHthOiAxfScpKCk7XG59IGNhdGNoKGVycm9yKSB7XG5cdGNvbnNvbGUuZXJyb3IoJ1lvdXIgSmF2YVNjcmlwdCBydW50aW1lIGRvZXMgbm90IHN1cHBvcnQgc29tZSBmZWF0dXJlcyB1c2VkIGJ5IHRoZSBjb2ZmZWUgY29tbWFuZC4gUGxlYXNlIHVzZSBOb2RlIDYgb3IgbGF0ZXIuJyk7XG5cdHByb2Nlc3MuZXhpdCgxKTtcbn1cblxudmFyIHBhdGg9cmVxdWlyZSgncGF0aCcpO1xudmFyIGZzPXJlcXVpcmUoJ2ZzJyk7XG5cbnZhciBwb3RlbnRpYWxQYXRocz1bXG5cdHBhdGguam9pbihwcm9jZXNzLmN3ZCgpLCksXG5cdHBhdGguam9pbihwcm9jZXNzLmN3ZCgpLCksXG5cdHBhdGguam9pbihwcm9jZXNzLmN3ZCgpLCksXG5cdHBhdGguam9pbihfX2Rpcm5hbWUsJy4uL2xpYi9jJylcbl07XG5cbmZvcih2YXIgaT0wLGxlbj1wb3RlbnRpYWxQYXRocy5sZW5ndGg7aTxsZW47aSsrKSB7XG5cdGlmKGZzLmV4aXN0c1N5bmMocG90ZW50aWFsUGF0aHNbaV0pKSB7XG5cdFx0cmVxdWlyZShwb3RlbnRpYWxQYXRoc1tpXSsnL2NvbW1hbmQnKS5ydW4oKTtcblx0XHRicmVhaztcblx0fVxufVxuXG5cbi8v5Lyg5Y+C5pWwXG52YXIgcHJvZ3JhbT1yZXF1aXJlKCdjb21tYW5kZXInKTtcblxucHJvZ3JhbVxuXHQuYXJndW1lbnRzKCc8ZmlsZT4nKVxuXHQub3B0aW9uKCctdSwgLS11c2VybmFtZSA8dXNlcm5hbWU+JywnVGhlIHVzZXIgdG8gYXV0aGVudGljYXRlIGFzJylcblx0Lm9wdGlvbignLXAsIC0tcGFzc3dvcmQgPHBhc3N3b3JkPicsJ1RoZSB1c2VycyBwYXNzd29yZCcpXG5cdFx0LmFjdGlvbihmdW5jdGlvbihmaWxlKSB7XG5cdFx0XHRjb25zb2xlLmxvZygndXNlcjogJXMgcGFzczogJXMgZmlsZTogJXMnLFxuXHRcdFx0XHRwcm9ncmFtLnVzZXJuYW1lLHByb2dyYW0ucGFzc3dvcmQsZmlsZSk7XG5cdFx0fSlcblx0XHQucGFyc2UocHJvY2Vzcy5hcmd2KTtcbiMjI1xuIiwiaW1wb3J0IGNoYWxrIGZyb20gJ2NoYWxrJ1xuI2NoYWxrID0gcmVxdWlyZSgnY2hhbGsnKVxuaW1wb3J0IHtmaWxlfSBmcm9tICdtbGliJ1xuXG4jIC0tLS0tLS0tLS0tLS0tLS0tcHVyZSBmdW5jdGlvbi0tLS0tLS0tLS0tLS0tLS0tXG4jIOWIpOaWreaYr+WQpuacieWRveS7pOWPguaVsCgtLeW8gOWni+eahOWRveS7pOWPguaVsCksIOayoeaciS0t55qE6YO95piv5paH5Lu2XG5leHBvcnQgaXNjbWQ9ICh4KS0+XG5cdC9eLS0vLnRlc3QgeFxuXG4jIOWkhOeQhmxw5ZG95LukXG5leHBvcnQgZG9scCA9IC0+XG5cdHN0cj0nJydcblx0IyDoiJTni5fpobnnm65cblx0YWxpYXMgbHA9XCJscGdcIlxuXHQnJydcblx0cmVnPS9hbGlhc1xccypscD1cImxwZ1wiL1xuXHRvaz1hd2FpdCBmaWxlLnVuaXF1ZWFkZCB7XG5cdFx0cmVnXG5cdFx0c3RyXG5cdFx0cGF0aDonfi8uenNocmMnXG5cdFx0I3BhdGg6ICdSRUFETUUubWQnXG5cdH1cblx0aWYgb2tcblx0XHRjb25zb2xlLmxvZyAoICflkb3ku6Torr7nva7kuLpscCcpLCAoY2hhbGsuZ3JlZW5CcmlnaHQgJ+aIkOWKnzogJyksJ+mcgOimgeWIt+aWsOmFjee9ruaJjeS8mueUn+aViCwg5o6o6I2Q5L2/55So5b+r5o236YeN5ZCv5ZG95LukOidcblx0XHRjb25zb2xlLmxvZyAoY2hhbGsuZ3JlZW5CcmlnaHQgJ2V4ZWMgenNoJyksKGNoYWxrLnllbGxvdyAnIyDph43lkK/lvZPliY10ZXJtaW5hbCcpXG5cblx0XHRjb25zb2xlLmxvZyAgJ+WmguaenOS4jeiDvemHjeWQr3Rlcm1pbmFsOiAnXG5cdFx0Y29uc29sZS4gbG9nIChjaGFsay5ncmVlbkJyaWdodCAnc291cmNlIH4vLnpzaHJjJyksIChjaGFsay55ZWxsb3cgJyMg5a+85YWl6YWN572u5paH5Lu25pa55rOV5LiAJylcblx0XHRjb25zb2xlLiBsb2cgKGNoYWxrLmdyZWVuQnJpZ2h0ICcuIH4vLnpzaHJjJyksIChjaGFsay55ZWxsb3cgJyMg5a+85YWl6YWN572u5paH5Lu25pa55rOV5LqMJylcblx0ZWxzZVxuXHRcdGNvbnNvbGUubG9nIChjaGFsay5yZWRCcmlnaHQgJ+ayoeaciScpLCAoICfov5vooYzorr7nva4sIOWPr+iDveaYr+mFjee9ruaWh+S7tuW3sue7j+WtmOWcqCcpLCAoY2hhbGsueWVsbG93ICfor7fmiYvlt6Xmo4Dmn6Xmlofku7Y6ICcpLCd+Ly56c2hyYydcblxuIyDlpITnkIbkuIDkuKpjc+aIluiAhWNvZmZlZeeahOWIneWni+WMllxuZGlyMWZpbGUgPSAocGF0aCktPlxuXHRwPSBwYXRoKycubHAnXG5cdCMg5Yik5pat5a+55bqU55uu5b2V5piv5ZCm5a2Y5ZyoLlxuXHRyZXR1cm4gaWYgKGZpbGUuZXhpc3QgcClcblx0IyDmlrDlu7rnm67lvZVcblx0YXdhaXQgZmlsZS5ta2RpciBwXG5cdCMgdG9kb+ivu+WPluaWh+S7tiwg5YiG5p6Q5Ye95pWwLCDlu7rnq4vlr7nlupTnmoTmtYvor5Xmlofku7Zcblx0IyDliJ3lp4vljJbmtYvor5Xmlofku7Zcblx0YXdhaXQgZmlsZS5jb3ZlciBwYXRoOnArJy9hYWEuMGkuY3Nvbicsc3RyOicnXG5cdGF3YWl0IGZpbGUuY292ZXIgcGF0aDpwKycvYWFhLjBvLmNzb24nLHN0cjonJ1xuXHRhd2FpdCBmaWxlLmNvdmVyIHBhdGg6cCsnL2N0by5jc29uJyxzdHI6JydcblxuIyDlpITnkIbliJ3lp4vljJbmtYvor5VkaXJcbmV4cG9ydCBkb2RpciA9IChwYXRoKS0+XG5cdHN0YXQ9IGF3YWl0IGZpbGUuZmlsZXR5cGUgcGF0aFxuXHRpZiBzdGF0LmlzZmlsZSAjIOaWh+S7tuWkhOeQhlxuXHRcdCMg5aSE55CGY29mZmVl5ZKMY3Ncblx0XHRyZXR1cm4gdW5sZXNzIC9cXC5jb2ZmZWUkfFxcLmNzJC8udGVzdCBwYXRoXG5cdFx0ZGlyMWZpbGUgcGF0aFxuXHRpZiBzdGF0LmlzZGlyICMg55uu5b2V5aSE55CGXG5cdFx0IyDpgJLlvZLlpITnkIbnm67lvZUsIOimgemBv+W8gOiHquWKqOeUn+aIkOeahOebruW9lSjlkI3np7Dph4zpnaLluKbngrnnmoQpXG5cdFx0cmV0dXJuIGlmIC9cXC4vLnRlc3QgcGF0aFxuXHRcdCMg6K+75Y+W55uu5b2VLCDpgJDpobnlpITnkIZcblx0XHRkb2RpciB4IGZvciB4IGluIGF3YWl0IGZpbGUucmVhZGlyIHBhdGhcblx0cmV0dXJuICMg6Zmk5LqGZmlsZeWSjGRpcuWPr+iDvei/mOacieWFtuS7luaDheWGtSwg5q+U5aaC56ym5Y+36ZO+5o6lXG5cbmV4cG9ydCBkZWZhdWx0IHtcblx0aXNjbWRcblx0ZG9scFxuXHRkb2RpclxufVxuXG5kb2RpciAnLidcbiIsIiMjI1xuICog5YWz5LqO6aKc6Imy55qE5bqTLCDov5nkuKrlupPkvJrooqvph43lhpksIOWboOS4umhzbOeahOS6ruW6puWIpOaWreWPr+S7peiupOS4uuaYr+mUmeeahC4g5omA5Lul5Y+v5Lul5L+d55WZaHNs55u45YWz55qE5Ye95pWwLCDkvYbmmK8sIOWunumZheW6lOeUqOWHveaVsOmcgOimgemHjeWGmS5cbiAqXG4jIyNcbmltcG9ydCBjb2xvcm5hbWUgZnJvbSAnLi9jb2xvcm5hbWUuY3NvbidcblxuZGFya2NvbG9yPSAncmdiYSg0NCw0NCw0NCwxKScgI2RhcmvnmoTln7rnoYDpopzoibJcbmxpZ2h0Y29sb3I9ICdyZ2JhKDIwMCwyMDAsMjAwLDEpJ1xuXG5cblxuXG5cbiMjIypcbiAqIC0tLS0tLS0tLS0tLS0tLS0tLS0tLeWNleeLrOWkhOeQhuWJjeaZr+aIluiAheiDjOaZrywg6YWN5ZCIZGFya2Nzc+eahOaAnei3ry0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLVxuICog5Lqu5bqm5Yik5pat6YC76L6RIGc+MTAwLCByPjIwMCwg5oiW6ICFZytyPjMwMFxuICog5Lqu5bqm5aSE55CG6YC76L6RIHQ9bWF4KDEwMC1nLCAyMDAtciksIHQrciwgdCtnLCB0K2JcbiAqXG4gKiDmmpfluqbliKTmlq3pgLvovpEgZzw2MCByPDEwMCBiPDE1MFxuICog5pqX5bqm5aSE55CG6YC76L6RIHQ9bWF4KGctNjAsIHItMTAwLCBiLTE1MCksIHItdCwgZy10LCBiLXRcbiAqXG4gIyMjXG5cblx0IyMjKlxuXHQgKiByZ2Jva1xuXHQgKiDliKTmlq3popzoibLmmK/lkKbpnIDopoHlpITnkIZcblx0ICogICog5Lqu5bqm5Yik5pat6YC76L6RIGc+MTAwLCByPjIwMCwg5oiW6ICFZytyPjMwMFxuXHQgKiAgKiDmmpfluqbliKTmlq3pgLvovpEgZzw2MCByPDEwMCBiPDE1MFxuXHQgKiDlj4LmlbBcblx0ICogXHRyZ2I6e3IsZyxifVxuXHQgKiAgaXNiYWNrOiDmmK/lkKbkuLrog4zmma/lm77niYdcblx0ICog6L+U5Zue5YC8XG5cdCAqIFx0XHR0cnVlOiDkuI3pnIDopoHlpITnkIZcblx0ICogXHRcdGZhbHNlOiDpnIDopoHlpITnkIZcblx0ICMjI1xuZXhwb3J0IHJnYm9rPSh7cmdiLGlzYmFjaz10cnVlfSktPlxuXHR7cixnLGJ9PXJnYlxuXHRpZiBpc2JhY2sgdGhlbiBcdGc8NjAgJiYgcjwxMDAgJiYgYjwxNTAgZWxzZSByK2c+MzAwXG5cdFx0XHQj5pqX5bqm5Yik5patXG5cdFx0XHQj5YWJ5piO5qih5byPLCDlpoLmnpzkuq7luqY8MC435YiZ6ZyA6KaB5aSE55CGLCDlkKbliJnlsLHkuI3pnIDopoHlpITnkIZcblxuXG4jIyMqXG4gKiDku45zdHLliLDosIPkuq7nmoTpopzoibJzdHJcbiAqICDov5nph4zlpITnkIbkuIDkuKpzdHlsZSBzdHJpbmfnmoTkuq7luqZcbiAqIOe6r+WHveaVsFxuICogc3RyOiDkuIDkuKrpopzoibLlrZfnrKbkuLJcbiAqIGlzYmFjazog5piv5ZCm5Li66IOM5pmvXG4gKiDlup/lvIMgcmV0dXJuIGZhbHNlIOW5tuS4jeWQiOeQhlxuIyMjXG5leHBvcnQgbHN0cj0oe3N0cixpc2JhY2s9dHJ1ZX0pIC0+XG5cdHJldHVybiBmYWxzZSBpZighc3RyKVxuXHRcblx0I+WmguaenOaYr2luaXRpYWwsIOmCo+S5iOebtOaOpeeUqOaIkeeahOWAvC5cblx0cmV0dXJuIChpZiBpc2JhY2sgdGhlbiBkYXJrY29sb3IgZWxzZSBsaWdodGNvbG9yKSBpZiBzdHI9PVwiaW5pdGlhbFwiXG5cdFxuXHQjZml4bWUg5pqC5pe25Zyo6L+Z6YeM5b+955WlY3NzIHZhciAgI3RvZG9cblx0cmV0dXJuIChpZiBpc2JhY2sgdGhlbiBkYXJrY29sb3IgZWxzZSBsaWdodGNvbG9yKWlmIC92YXJcXCgtLS8udGVzdChzdHIpXG5cblx0bD1scmdiIHtyZ2I6IHN0cjJyZ2Ioc3RyKSxpc2JhY2t9XG5cdHJnYjJzdHIobClcblxuIyMjKlxuXHQgKiBsaWdodHJnYuaguOW/g+S6ruW6puWkhOeQhuWHveaVsFxuXHQgKiAgKiDkuq7luqblpITnkIbpgLvovpEgdD1tYXgoMTAwLWcsIDIwMC1yKSwgdCtyLCB0K2csIHQrYlxuXHQgKiAgKiDmmpfluqblpITnkIbpgLvovpEgdD1tYXgoZy02MCwgci0xMDAsIGItMTUwKSwgci10LCBnLXQsIGItdFxuXHQgKiDlup/lvINyZXR1cm4gZmFsc2XlubbkuI3lkIjnkIZcbiMjI1xuZXhwb3J0IGxyZ2I9KHtyZ2IsaXNiYWNrPXRydWV9KSAtPlxuXHRyZXR1cm4gcmdiIGlmIHJnYm9rIHtyZ2IsaXNiYWNrfVxuXHR7cixnLGJ9PXJnYlxuXHRpZiAoaXNiYWNrKSAgI+WkhOeQhuaal+m7keiDjOaZr1xuXHRcdHQ9IE1hdGgubWF4KGctNjAsci0xMDAsIGItMTUwKVxuXHRcdHItPXRcblx0XHRnLT10XG5cdFx0Yi09dFxuXHRlbHNlICAj5aSE55CG5piO5Lqu5YmN5pmvXG5cdFx0dD0gTWF0aC5tYXgoMTAwLWcsMjAwLXIpXG5cdFx0cis9dFxuXHRcdGcrPXRcblx0XHRiKz10XG5cdHtyLGcsYn1cblxuIyMjKlxuICog5LuOc3Ry5Yiw6LCD5Lqu55qE6aKc6Imyc3RyXG4gKiAg6L+Z6YeM5aSE55CG5LiA5Liqc3R5bGUgc3RyaW5n55qE5Lqu5bqmXG4gKiDnuq/lh73mlbBcbiAqIHN0cjog5LiA5Liq6aKc6Imy5a2X56ym5LiyXG4gKiBpc2JhY2s6IOaYr+WQpuS4uuiDjOaZr1xuICog5bqf5byDIHJldHVybiBmYWxzZSDlubbkuI3lkIjnkIZcbiMjI1xubGlnaHRzdHI9KHtzdHIsaXNiYWNrPXRydWV9KSAtPlxuXHRyZXR1cm4gZmFsc2UgaWYoIXN0cilcblx0XG5cdCPlpoLmnpzmmK9pbml0aWFsLCDpgqPkuYjnm7TmjqXnlKjmiJHnmoTlgLwuXG5cdHJldHVybiAoaWYgaXNiYWNrIHRoZW4gZGFya2NvbG9yIGVsc2UgbGlnaHRjb2xvcikgaWYgc3RyPT1cImluaXRpYWxcIlxuXHRcblx0I2ZpeG1lIOaaguaXtuWcqOi/memHjOW/veeVpWNzcyB2YXIgICN0b2RvXG5cdHJldHVybiAoaWYgaXNiYWNrIHRoZW4gZGFya2NvbG9yIGVsc2UgbGlnaHRjb2xvcilpZiAvdmFyXFwoLS0vLnRlc3Qoc3RyKVxuXHRsPWxpZ2h0cmdiIHtyZ2I6IHN0cjJyZ2Ioc3RyKSxpc2JhY2t9XG5cdGlmIGwgdGhlbiByZ2Iyc3RyKGwpIGVsc2UgZmFsc2VcblxuXG5cbiMjIypcblx0ICogbGlnaHRyZ2LmoLjlv4Pkuq7luqblpITnkIblh73mlbBcblx0ICogICog5Lqu5bqm5aSE55CG6YC76L6RIHQ9bWF4KDEwMC1nLCAyMDAtciksIHQrciwgdCtnLCB0K2Jcblx0ICogICog5pqX5bqm5aSE55CG6YC76L6RIHQ9bWF4KGctNjAsIHItMTAwLCBiLTE1MCksIHItdCwgZy10LCBiLXRcblx0ICog5bqf5byDcmV0dXJuIGZhbHNl5bm25LiN5ZCI55CGXG4jIyNcbmxpZ2h0cmdiPSh7cmdiLGlzYmFjaz10cnVlfSkgLT5cblx0cmV0dXJuIGZhbHNlIGlmKHJnYm9rKHtyZ2IsaXNiYWNrfSkpXG5cdHtyLGcsYn09cmdiXG5cdGlmIChpc2JhY2spICAj5aSE55CG5pqX6buR6IOM5pmvXG5cdFx0dD0gTWF0aC5tYXgoZy02MCxyLTEwMCwgYi0xNTApXG5cdFx0ci09dFxuXHRcdGctPXRcblx0XHRiLT10XG5cdGVsc2UgICPlpITnkIbmmI7kuq7liY3mma9cblx0XHR0PSBNYXRoLm1heCgxMDAtZywyMDAtcilcblx0XHRyKz10XG5cdFx0Zys9dFxuXHRcdGIrPXRcblx0e3IsZyxifVxuXG5cblxuXG5cbiMjIypcbiAqIC0tLS0tLS0tLS0tLS0t5a+55q+U5bqm6YC76L6RLCDphY3lkIhkYXJrc3R5bGUtLS0tLS0tLS0tLS0tLS0tLVxuICog6L+Z6YeM55qE6K6h566X5pyq5b+F5ZCI6YCCLlxuIyMjXG5cblxuXG5cblx0XHQjIyMqXG5cdFx0ICog5LuOc3Ry5Yiw5Lqu5bqmXG5cdFx0ICMjI1xuZXhwb3J0IGdldGxpZ2h0PSAoc3RyKSAtPlxuXHRnZXRyZ2JsaWdodCBzdHIycmdiIHN0clxuXHRcdFxuZXhwb3J0IGdldGRhcms9KHN0cikgLT5cblx0Z2V0cmdiZGFyayBzdHIycmdiIHN0clxuXHRcdFxuXG5cblxuIyMjKlxuXHQqIOaLv+WIsOinhuinieS6ruW6plxuXHQqIOeUqOadpeWIpOaWreminOiJsuaYr+WQpuWkn+S6rlxuXHQqIOacquadpeeUqOiNp+WFieS9k+WPkeWFieiDvemHj+adpeiuoeeul+aVsOWAvC5cbiMjI1xuZ2V0cmdibGlnaHQ9ICh7cixnLGJ9KS0+XG5cdHI9cioxLjJcblx0Zz1nKjEuN1xuXHRtbGlnaHQoe3IsZyxifSkubGlnaHRcblxuIyMjKlxuXHQqIOaLv+WIsOinhuinieaal+W6plxuXHQqIOeUqOadpeWIpOaWreminOiJsuaYr+WQpuWkn+aal1xuXHQqXG4jIyNcbmdldHJnYmRhcms9KHtyLGcsYn0pLT5cblx0cj1yKjEuM1xuXHRnPWcqMS4yXG5cdG1saWdodCh7cixnLGJ9KS5saWdodFxuXG5tbGlnaHQ9KHtyLGcsYn0pLT5cblx0aWYgciA+IGdcblx0XHRiMT1yXG5cdFx0YjI9Z1xuXHRlbHNlXG5cdFx0YjE9Z1xuXHRcdGIyPXJcblx0XG5cdGlmIGIgPCBiMlxuXHRcdGIzPWJcblx0ZWxzZSBpZiBiIDwgYjFcblx0XHRiMz1iMlxuXHRcdGIyPWJcblx0ZWxzZVxuXHRcdGIzPWIyXG5cdFx0YjI9YjFcblx0XHRiMT1iXG5cdFx0XHQjIOi/memHjOW+iOS4jeS4peiwqCwg5Lqu5bqm55qE5b+D55CG6ZiI6ZmQ5rWL6YeP5oiW6K646IO96K6p6L+Z5Liq6K6h566X5pu057K+56GuLlxuXHRsaWdodDogKGIxK2IyLzYpKjIvMyxtYXg6IFtiMSxiMixiM11cblx0XG5cblxuXG5cblxuXG5cblxuIyMjXG5cdCog5Z+656GA5bel5YW35Ye95pWwLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLS0tLVxuXHQqIOWIpOaWreS4pOS4qnJnYnN0cuWunumZheebuOetiVxuIyMjXG5cbmV4cG9ydCBzYW1lcmdic3RyPShyZ2JzdHIxLHJnYnN0cjIpLT5cblx0e3I6IHIxLGc6IGcxLGI6IGIxfT1zdHIycmdiKHJnYnN0cjEpXG5cdHtyOiByMixnOiBnMixiOiBiMn09c3RyMnJnYihyZ2JzdHIyKVxuXHRyMT09cjImJmcxPT1nMiYmYjE9PWIyXG5cblxuIyMjKlxuKiAz56eN5oOF5Ya1XG4qIDEuIHJnYuaDheWGtTog5LuOXCJyZ2IoMTU0LCAxNTQsIDApXCLovazljJbkuLo6IHtyOjE1NCxnOjE1NCxiOjAsfVxuKiBcInJnYmEoMCwgMCwgMCwgMClcIiDov5jpnIDlhbzlrrnmraTnp43mg4XlhrVcbiogMi4gMTbov5vliLbmg4XlhrU6ICAjZmMyICAjZmMyOSAjZmMyOTI5MjMgI2ZjMjkyOVxuKiAzLiDpopzoibLlkI06IHdoaXRlLCBibGFjaywgb3JnYW5nZVxuKiMjI1xuXG5zdHIycmdiPShzdHIpLT5cblxuXHQjXCJyZ2IoMTU0LCAxNTQsIDApXCJcblx0cmVnPS9yZ2JhP1xcKChbMC05XXsxLDN9KSwgPyhbMC05XXsxLDN9KSwgPyhbMC05XXsxLDN9KSgsID8oWzAtOS4lXSspKT9cXCkvaVxuXHRjPXJlZy5leGVjIHN0clxuXG5cdCMxNui/m+WItuminOiJslxuXHRjPz1oZXgycmdiKHN0cilcblxuXHQj5YaN5Yik5pat5ZCN5a2X6aKc6ImyOlxuXHRjPz0gY29sb3JuYW1lW3N0cl0gJiYgaGV4MnJnYihoZXgpXG5cdFxuXHRyZXR1cm4gZmFsc2UgdW5sZXNzIGM/XG5cblxuXHQj6YCP5piO5bqmXG5cdGE9IGlmIGNbNV0/IHRoZW4gY1s1XSBlbHNlIDFcblx0e3I6ICtjWzFdLGc6ICtjWzJdLGI6ICtjWzNdLGF9ICPov5nph4znlKgr5piv5Li65LqG5oqK5a2X56ym5Liy6L2s5Li65pWw5a2XLCDkuI3nhLbkvJrlj5HnlJ/lvojpmpDolL3nmoRidWcuXG5cblxuXHQjIyNcblx0ICog5LuOcmdi5Yiwc3RyXG5cdCAqIyMjXG5yZ2Iyc3RyPSh7cixnLGIsYT0xfSkgLT5cblx0aWYgYT4wLjk5IHRoZW4gIFwicmdiKFwiK3IrXCIsXCIrZytcIixcIitiK1wiKVwiIGVsc2UgXCJyZ2JhKFwiK3IrXCIsXCIrZytcIixcIitiK1wiLFwiK2ErXCIpXCJcblxuXG5cdCMjI1xuXHQgKiDku44xNui/m+WItuWIsHJnYlxuXHQgKiAgI2ZjMiAgI2ZjMjkgI2ZjMjkyOTIzICNmYzI5MjksIDMsNCw2LDjpg73mnInlj6/og71cblx0IyMjXG5cblxuaGV4MnJnYj0oc3RyKSAtPlxuXHRyZWc9LyMoW14jXSopL2kgI+WIpOaWrSM2MmZhODnov5nnp43nsbvkvLzmlrnlvI/nmoTpopzoibLlgLxcblx0Yz1yZWcuZXhlYyhzdHIpXG5cdHJldHVybiBmYWxzZSB1bmxlc3MgYz9bMV0/IFx0I2Mx5LiN5a2Y5Zyo6ZyA6KaBcmV0dXJuXG5cdHg9Y1sxXVxuXHR5PVtdXG5cdHN3aXRjaCB4Lmxlbmd0aFxuXHRcdHdoZW4gM1xuXHRcdFx0eVsxXT1wYXJzZUludCgnJyt4WzBdK3hbMF0sMTYpXG5cdFx0XHR5WzJdPXBhcnNlSW50KCcnK3hbMV0reFsxXSwxNilcblx0XHRcdHlbM109cGFyc2VJbnQoJycreFsyXSt4WzJdLDE2KVxuXHRcdHdoZW4gNFxuXHRcdFx0I3lbMV09cGFyc2VJbnQoeFswXSwxNik7ICAvMjAyMDEwMjjkv67mraPplJnor68sIOi/meS4quS8muaKimZmZui9rOWMluS4uiByMTVnMTViMTUsIOWFtuWunuW6lOivpeaYr3IyNTVnMjU1YjI1NVxuXHRcdFx0eVsxXT1wYXJzZUludCgnJyt4WzBdK3hbMF0sMTYpXG5cdFx0XHR5WzJdPXBhcnNlSW50KCcnK3hbMV0reFsxXSwxNilcblx0XHRcdHlbM109cGFyc2VJbnQoJycreFsyXSt4WzJdLDE2KVxuXHRcdFx0eVs1XT1wYXJzZUludCh4WzNdLDE2KS8yNTVcblx0XHR3aGVuIDZcblx0XHRcdHlbMV09cGFyc2VJbnQoJycreFswXSt4WzFdLDE2KVxuXHRcdFx0eVsyXT1wYXJzZUludCgnJyt4WzJdK3hbM10sMTYpXG5cdFx0XHR5WzNdPXBhcnNlSW50KCcnK3hbNF0reFs1XSwxNilcblx0XHR3aGVuIDhcblx0XHRcdHlbMV09cGFyc2VJbnQoJycreFswXSt4WzFdLDE2KVxuXHRcdFx0eVsyXT1wYXJzZUludCgnJyt4WzJdK3hbM10sMTYpXG5cdFx0XHR5WzNdPXBhcnNlSW50KCcnK3hbNF0reFs1XSwxNilcblx0XHRcdHlbNV09cGFyc2VJbnQoJycreFs2XSt4WzddLDE2KS8yNTVcblx0XHQj5LiN5ruh6Laz5LiK6Z2i5Zub5Liq5oOF5Ya1LCDnm7TmjqXov5Tlm55mYWxzZVxuXHRcdGVsc2UgcmV0dXJuIGZhbHNlXG5cdHlcblxuXHQjY29uc29sZS5sb2coc3RyMnJnYignd2hpdGUnKSk7ICN0ZXN0XG5cblxuXG5cblxuXG50ZXN0ID0gLT5cblx0cmVzdWx0PXdpbmRvdy5sdW9jbHIubGlnaHRzdHIgc3RyOid3aGl0ZScsIGlzYmFjazp0cnVlXG5cdGNvbnNvbGUubG9nIHtyZXN1bHR9XG5cblxuZXhwb3J0IGRlZmF1bHQge1xuXHQjbGlnaHRzdHJcblx0I2xpZ2h0cmdiXG5cdGxzdHJcblx0bHJnYlxuXHRnZXRsaWdodFxuXHRnZXRkYXJrXG5cdHNhbWVyZ2JzdHJcblxufVxuIiwiI2ltcG9ydCB7bG9nfSBmcm9tICcuL2xvZy5qcydcblxuaW1wb3J0IGZzIGZyb20gJ2ZzL3Byb21pc2VzJ1xuaW1wb3J0IGZ0cyBmcm9tICdmcydcblxuaW1wb3J0IG9zIGZyb20gJ29zJ1xuXG5ob21lID0gb3MuaG9tZWRpcigpXG5sb2c9Y29uc29sZS5sb2dcblxuXG4jIHRvZG8g5L+d5a2Y5paH5Lu2XG5leHBvcnQgc2F2ZWZpbGU9KHtqc29uLGZpbGVuYW1lPVwidGVtcHRlc3QudHh0XCJ9KS0+XG5cdGZzLndyaXRlRmlsZSBmaWxlbmFtZSxKU09OLnN0cmluZ2lmeShqc29uKSwoZXJyKS0+XG5cdFx0bG9nICfmlofku7blhpnlhaUnLGZpbGVuYW1lXG5cdFx0bG9nIGVyciBpZiBlcnJcblxuIyDmi7/liLDnnJ/mraPnmoRwYXRoXG5leHBvcnQgcmVhbHBhdGg9KHBhdGgpLT5cblx0IyBob21l5paH5Lu25aS5fiwgbm9kZeS4jeiupCwg6KaB6LCD5pW05Li6b3MuaG9tZWRpclxuXHRpZiAvXn4vLnRlc3QgcGF0aCB0aGVuIHBhdGgucmVwbGFjZSAvXn4vLCBob21lICBlbHNlIHBhdGhcblxuXG4jIOWKoOaWh+S7tuWGheWuuSwg6KaG55uW5byPXG5leHBvcnQgY292ZXI9KHtwYXRoLCBzdHIsIG9wdGlvbnM9ZW5jb2Rpbmc6J3V0ZjgnfSkgLT5cblxuXHQjIOimhuebluW8j+WGmeWFpVxuXHRhd2FpdCBmcy53cml0ZUZpbGUgKHJlYWxwYXRoIHBhdGgpLCBzdHIsIG9wdGlvbnNcblx0dHJ1ZVxuIyDov73liqDmlofku7blhoXlrrksIOS/neivgei/veWKoOWGheWuueS4jemHjeWkjVxuZXhwb3J0IHVuaXF1ZWFkZD0oe3BhdGgsIHJlZywgc3RyLCBvcHRpb25zPWVuY29kaW5nOid1dGY4J30pIC0+XG5cblx0IyBob21l5paH5Lu25aS5fiwgbm9kZeS4jeiupCwg6KaB6LCD5pW05Li6b3MuaG9tZWRpclxuXHRycGF0aCA9cmVhbHBhdGggcGF0aFxuXG5cdCMg6K+75Y+W5paH5Lu2XG5cdGZzdHI9YXdhaXQgZnMucmVhZEZpbGUgcnBhdGgsIG9wdGlvbnNcblx0IyDmraPliJnliKTmlq1cblx0cmV0dXJuIGZhbHNlIGlmIHJlZy50ZXN0IGZzdHJcblxuXHQjIOS4jemHjeWkjeWwseaPkuWFpVxuXHRhd2FpdCBmcy5hcHBlbmRGaWxlIHJwYXRoLCBzdHIgPyByZWcuc291cmNlLCBvcHRpb25zXG5cdHRydWVcblxuIyDliKTmlq3mmK/mlofku7Yv55uu5b2VXG5leHBvcnQgZmlsZXR5cGUgPSAocGF0aCktPlxuXHRzdGF0PWF3YWl0IGZzLmxzdGF0IHJlYWxwYXRoIHBhdGhcblx0cmV0dXJuIGlzZmlsZTogc3RhdC5pc0ZpbGUoKSwgaXNkaXI6IHN0YXQuaXNEaXJlY3RvcnkoKVxuXG4jIOWIpOaWreaWh+S7ti/nm67lvZXmmK/lkKblrZjlnKhcbmV4cG9ydCBleGlzdCA9IChwYXRoKS0+XG5cdGZ0cy5leGlzdHNTeW5jIHJlYWxwYXRoIHBhdGhcblxuIyDlu7rkuIDkuKrnm67lvZVcbmV4cG9ydCBta2Rpcj0ocGF0aCktPlxuXHRhd2FpdCBmcy5ta2RpciAocmVhbHBhdGggcGF0aCkscmVjdXJzaXZlOnRydWVcblxuIyDlpoLmnpzkuI3lrZjlnKjlsLHlu7rkuIDkuKrnm67lvZVcbmV4cG9ydCBuZXdkaXI9KHBhdGgpLT5cblx0cmV0dXJuIGZhbHNlIGlmIGV4aXN0IHBhdGhcblx0YXdhaXQgZnMubWtkaXIgcmVhbHBhdGggcGF0aFxuXG4jIOivu+WPluebruW9lVxuZXhwb3J0IHJlYWRpciA9IChwYXRoKS0+XG5cdGF3YWl0IGZzLnJlYWRkaXIgcmVhbHBhdGggcGF0aFxuIyDor7vlj5bmlofku7ZcbmV4cG9ydCByZWFkZmlsZSA9IChwYXRoKS0+XG5cdGZzdHI9YXdhaXQgZnMucmVhZEZpbGUgKHJlYWxwYXRoIHBhdGgpLCBlbmNvZGluZzondXRmOCdcblxuXG4jIHRvZG8g6K+75Y+W55uu5b2V5Lit5omA5pyJ6Zeu6aKYLCDmt7HluqbpgY3ljoZcbmV4cG9ydCBhbGxmaWxlID0gKHtwYXRoLCByZWc9ZmFsc2Usb3B0aW9ucz1lbmNvZGluZzondXRmOCd9KS0+XG5cdCMg5aaC5p6c5rKh5LygcmVnLCDpgqPkuYjlsLHmmK/opoHmiYDmnInmlofku7Zcblx0cmVhZGRpcihwYXRoLCBvcHRpb25zKVxuXG5cblxuXG5leHBvcnQgZGVmYXVsdCB7XG5cdHNhdmVmaWxlXG5cdGNvdmVyXG5cdHVuaXF1ZWFkZFxuXHRmaWxldHlwZVxuXHRleGlzdFxuXHRta2RpclxuXHRyZWFkaXJcblx0cmVhZGZpbGVcblx0YWxsZmlsZVxufVxuIiwiZXhwb3J0IHtkZWZhdWx0IGFzIGNvbG9yfSBmcm9tICcuL2NvbG9yLmNzJ1xuZXhwb3J0IHtkZWZhdWx0IGFzIHJhbmRvbX0gZnJvbSBcdCcuL3JhbmRvbS5jcydcbmV4cG9ydCB7ZGVmYXVsdCBhcyB0aW1lfSBmcm9tIFx0Jy4vdGltZS5jcydcbmV4cG9ydCB7ZGVmYXVsdCBhcyBmaWxlfSBmcm9tIFx0Jy4vZmlsZS5jcydcbiIsIlxuIyMjXG4qIOaLv+WIsOaVtOaVsOeJiOacrOeahOmaj+acuuaVsFxuKiDlm6DkuLpyYW5kb23kuI3ljIXlkKsxLCDlm6DmraQsIOi/meS4qumaj+acuuaVsOS5n+S4jeWMheWQq21heFxuKiDlm6DmraTkvKDlhaXmlbDnu4TnmoTplb/luqbmraPlpb3lkIjpgILkvZzkuLrmlbDnu4TnmoTntKLlvJVcbiMjI1xuaW1wb3J0IHtsc3RyfSBmcm9tICcuL2NvbG9yLmNzJ1xuXG5leHBvcnQgcmFuZG9taW50PShtYXgpIC0+XG5cdChNYXRoLnJhbmRvbSgpKm1heCk+PjBcblxuIyMjXG7mlbDnu4TkuK3lj5bkuIDkuKrpmo/mnLplbGVtZW50XG4jIyNcbmV4cG9ydCByYW5kb21lbWJlciA9IChwKSAtPlxuXHRwW3JhbmRvbWludCBwLmxlbmd0aF1cblxuIyDpmo/mnLrkuIDkuKrpopzoibJcbmV4cG9ydCByYW5kb21jb2xvciA9IC0+XG5cdFwicmdiKFwiKyAocmFuZG9taW50IDI1NikrXCIsXCIrKHJhbmRvbWludCAyNTYgKStcIixcIisocmFuZG9taW50IDI1NikrXCIpXCJcblxuXG4jIOmaj+acuuS4gOS4quaYjuS6ruminOiJslxuZXhwb3J0IHJhbmRvbWxpZ2h0ID0gLT5cblx0bHN0clxuXHRcdHN0cjpyYW5kb21jb2xvciAwXG5cdFx0aXNiYWNrOmZhbHNlXG5cblxuXG5cbmV4cG9ydCBkZWZhdWx0IHtcblx0cmFuZG9taW50XG5cdHJhbmRvbWVtYmVyXG5cdHJhbmRvbWNvbG9yXG5cdHJhbmRvbWxpZ2h0XG59XG4iLCJleHBvcnQgc2xlZXAgPSAobXMpIC0+XG5cdG5ldyBQcm9taXNlIChyZXNvbHZlKSAtPlxuXHRcdHNldFRpbWVvdXQgcmVzb2x2ZSwgbXNcblxuXG5leHBvcnQgZ2V0aGhtbSA9KHRpbWVzdGFtcD1EYXRlLm5vdygpKS0+XG5cdHQ9bmV3IERhdGUoK3RpbWVzdGFtcClcblx0dC5nZXRIb3VycygpKyc6Jyt0LmdldE1pbnV0ZXMoKVxuXG5leHBvcnQgZ2V0aGhtbXNzID0odGltZXN0YW1wPURhdGUubm93KCkpLT5cblx0dD1uZXcgRGF0ZSgrdGltZXN0YW1wKVxuXHR0LmdldEhvdXJzKCkrJzonK3QuZ2V0TWludXRlcygpKyc6Jyt0LmdldFNlY29uZHMoKVxuXG5cblxuXG5leHBvcnQgZGVmYXVsdCB7XG5cdHNsZWVwXG5cdGdldGhobW1cblx0Z2V0aGhtbXNzXG59XG4iLCJtb2R1bGUuZXhwb3J0cyA9IHtcImJsYWNrXCI6XCIjMDAwXCIsXCJzaWx2ZXJcIjpcIiNjMGMwYzBcIixcImdyYXlcIjpcIiM4MDgwODBcIixcIndoaXRlXCI6XCIjZmZmXCIsXCJtYXJvb25cIjpcIiM4MDAwMDBcIixcInJlZFwiOlwiI2ZmMDAwMFwiLFwicHVycGxlXCI6XCIjODAwMDgwXCIsXCJmdWNoc2lhXCI6XCIjZmYwMGZmXCIsXCJncmVlblwiOlwiIzAwODAwMFwiLFwibGltZVwiOlwiIzAwZmYwMFwiLFwib2xpdmVcIjpcIiM4MDgwMDBcIixcInllbGxvd1wiOlwiI2ZmZmYwMFwiLFwibmF2eVwiOlwiIzAwMDA4MFwiLFwiYmx1ZVwiOlwiIzAwMDBmZlwiLFwidGVhbFwiOlwiIzAwODA4MFwiLFwiYXF1YVwiOlwiIzAwZmZmZlwiLFwib3JhbmdlXCI6XCIjZmZhNTAwXCIsXCJhbGljZWJsdWVcIjpcIiNmMGY4ZmZcIixcImFudGlxdWV3aGl0ZVwiOlwiI2ZhZWJkN1wiLFwiYXF1YW1hcmluZVwiOlwiIzdmZmZkNFwiLFwiYXp1cmVcIjpcIiNmMGZmZmZcIixcImJlaWdlXCI6XCIjZjVmNWRjXCIsXCJiaXNxdWVcIjpcIiNmZmU0YzRcIixcImJsYW5jaGVkYWxtb25kXCI6XCIjZmZlYmNkXCIsXCJibHVldmlvbGV0XCI6XCIjOGEyYmUyXCIsXCJicm93blwiOlwiI2E1MmEyYVwiLFwiYnVybHl3b29kXCI6XCIjZGViODg3XCIsXCJjYWRldGJsdWVcIjpcIiM1ZjllYTBcIixcImNoYXJ0cmV1c2VcIjpcIiM3ZmZmMDBcIixcImNob2NvbGF0ZVwiOlwiI2QyNjkxZVwiLFwiY29yYWxcIjpcIiNmZjdmNTBcIixcImNvcm5mbG93ZXJibHVlXCI6XCIjNjQ5NWVkXCIsXCJjb3Juc2lsa1wiOlwiI2ZmZjhkY1wiLFwiY3JpbXNvblwiOlwiI2RjMTQzY1wiLFwiY3lhblwiOlwiIzAwZmZmZlwiLFwiZGFya2JsdWVcIjpcIiMwMDAwOGJcIixcImRhcmtjeWFuXCI6XCIjMDA4YjhiXCIsXCJkYXJrZ29sZGVucm9kXCI6XCIjYjg4NjBiXCIsXCJkYXJrZ3JheVwiOlwiI2E5YTlhOVwiLFwiZGFya2dyZWVuXCI6XCIjMDA2NDAwXCIsXCJkYXJrZ3JleVwiOlwiI2E5YTlhOVwiLFwiZGFya2toYWtpXCI6XCIjYmRiNzZiXCIsXCJkYXJrbWFnZW50YVwiOlwiIzhiMDA4YlwiLFwiZGFya29saXZlZ3JlZW5cIjpcIiM1NTZiMmZcIixcImRhcmtvcmFuZ2VcIjpcIiNmZjhjMDBcIixcImRhcmtvcmNoaWRcIjpcIiM5OTMyY2NcIixcImRhcmtyZWRcIjpcIiM4YjAwMDBcIixcImRhcmtzYWxtb25cIjpcIiNlOTk2N2FcIixcImRhcmtzZWFncmVlblwiOlwiIzhmYmM4ZlwiLFwiZGFya3NsYXRlYmx1ZVwiOlwiIzQ4M2Q4YlwiLFwiZGFya3NsYXRlZ3JheVwiOlwiIzJmNGY0ZlwiLFwiZGFya3NsYXRlZ3JleVwiOlwiIzJmNGY0ZlwiLFwiZGFya3R1cnF1b2lzZVwiOlwiIzAwY2VkMVwiLFwiZGFya3Zpb2xldFwiOlwiIzk0MDBkM1wiLFwiZGVlcHBpbmtcIjpcIiNmZjE0OTNcIixcImRlZXBza3libHVlXCI6XCIjMDBiZmZmXCIsXCJkaW1ncmF5XCI6XCIjNjk2OTY5XCIsXCJkaW1ncmV5XCI6XCIjNjk2OTY5XCIsXCJkb2RnZXJibHVlXCI6XCIjMWU5MGZmXCIsXCJmaXJlYnJpY2tcIjpcIiNiMjIyMjJcIixcImZsb3JhbHdoaXRlXCI6XCIjZmZmYWYwXCIsXCJmb3Jlc3RncmVlblwiOlwiIzIyOGIyMlwiLFwiZ2FpbnNib3JvXCI6XCIjZGNkY2RjXCIsXCJnaG9zdHdoaXRlXCI6XCIjZjhmOGZmXCIsXCJnb2xkXCI6XCIjZmZkNzAwXCIsXCJnb2xkZW5yb2RcIjpcIiNkYWE1MjBcIixcImdyZWVueWVsbG93XCI6XCIjYWRmZjJmXCIsXCJncmV5XCI6XCIjODA4MDgwXCIsXCJob25leWRld1wiOlwiI2YwZmZmMFwiLFwiaG90cGlua1wiOlwiI2ZmNjliNFwiLFwiaW5kaWFucmVkXCI6XCIjY2Q1YzVjXCIsXCJpbmRpZ29cIjpcIiM0YjAwODJcIixcIml2b3J5XCI6XCIjZmZmZmYwXCIsXCJraGFraVwiOlwiI2YwZTY4Y1wiLFwibGF2ZW5kZXJcIjpcIiNlNmU2ZmFcIixcImxhdmVuZGVyYmx1c2hcIjpcIiNmZmYwZjVcIixcImxhd25ncmVlblwiOlwiIzdjZmMwMFwiLFwibGVtb25jaGlmZm9uXCI6XCIjZmZmYWNkXCIsXCJsaWdodGJsdWVcIjpcIiNhZGQ4ZTZcIixcImxpZ2h0Y29yYWxcIjpcIiNmMDgwODBcIixcImxpZ2h0Y3lhblwiOlwiI2UwZmZmZlwiLFwibGlnaHRnb2xkZW5yb2R5ZWxsb3dcIjpcIiNmYWZhZDJcIixcImxpZ2h0Z3JheVwiOlwiI2QzZDNkM1wiLFwibGlnaHRncmVlblwiOlwiIzkwZWU5MFwiLFwibGlnaHRncmV5XCI6XCIjZDNkM2QzXCIsXCJsaWdodHBpbmtcIjpcIiNmZmI2YzFcIixcImxpZ2h0c2FsbW9uXCI6XCIjZmZhMDdhXCIsXCJsaWdodHNlYWdyZWVuXCI6XCIjMjBiMmFhXCIsXCJsaWdodHNreWJsdWVcIjpcIiM4N2NlZmFcIixcImxpZ2h0c2xhdGVncmF5XCI6XCIjNzc4ODk5XCIsXCJsaWdodHNsYXRlZ3JleVwiOlwiIzc3ODg5OVwiLFwibGlnaHRzdGVlbGJsdWVcIjpcIiNiMGM0ZGVcIixcImxpZ2h0eWVsbG93XCI6XCIjZmZmZmUwXCIsXCJsaW1lZ3JlZW5cIjpcIiMzMmNkMzJcIixcImxpbmVuXCI6XCIjZmFmMGU2XCIsXCJtYWdlbnRhXCI6XCIjZmYwMGZmXCIsXCJtZWRpdW1hcXVhbWFyaW5lXCI6XCIjNjZjZGFhXCIsXCJtZWRpdW1ibHVlXCI6XCIjMDAwMGNkXCIsXCJtZWRpdW1vcmNoaWRcIjpcIiNiYTU1ZDNcIixcIm1lZGl1bXB1cnBsZVwiOlwiIzkzNzBkYlwiLFwibWVkaXVtc2VhZ3JlZW5cIjpcIiMzY2IzNzFcIixcIm1lZGl1bXNsYXRlYmx1ZVwiOlwiIzdiNjhlZVwiLFwibWVkaXVtc3ByaW5nZ3JlZW5cIjpcIiMwMGZhOWFcIixcIm1lZGl1bXR1cnF1b2lzZVwiOlwiIzQ4ZDFjY1wiLFwibWVkaXVtdmlvbGV0cmVkXCI6XCIjYzcxNTg1XCIsXCJtaWRuaWdodGJsdWVcIjpcIiMxOTE5NzBcIixcIm1pbnRjcmVhbVwiOlwiI2Y1ZmZmYVwiLFwibWlzdHlyb3NlXCI6XCIjZmZlNGUxXCIsXCJtb2NjYXNpblwiOlwiI2ZmZTRiNVwiLFwibmF2YWpvd2hpdGVcIjpcIiNmZmRlYWRcIixcIm9sZGxhY2VcIjpcIiNmZGY1ZTZcIixcIm9saXZlZHJhYlwiOlwiIzZiOGUyM1wiLFwib3JhbmdlcmVkXCI6XCIjZmY0NTAwXCIsXCJvcmNoaWRcIjpcIiNkYTcwZDZcIixcInBhbGVnb2xkZW5yb2RcIjpcIiNlZWU4YWFcIixcInBhbGVncmVlblwiOlwiIzk4ZmI5OFwiLFwicGFsZXR1cnF1b2lzZVwiOlwiI2FmZWVlZVwiLFwicGFsZXZpb2xldHJlZFwiOlwiI2RiNzA5M1wiLFwicGFwYXlhd2hpcFwiOlwiI2ZmZWZkNVwiLFwicGVhY2hwdWZmXCI6XCIjZmZkYWI5XCIsXCJwZXJ1XCI6XCIjY2Q4NTNmXCIsXCJwaW5rXCI6XCIjZmZjMGNiXCIsXCJwbHVtXCI6XCIjZGRhMGRkXCIsXCJwb3dkZXJibHVlXCI6XCIjYjBlMGU2XCIsXCJyb3N5YnJvd25cIjpcIiNiYzhmOGZcIixcInJveWFsYmx1ZVwiOlwiIzQxNjllMVwiLFwic2FkZGxlYnJvd25cIjpcIiM4YjQ1MTNcIixcInNhbG1vblwiOlwiI2ZhODA3MlwiLFwic2FuZHlicm93blwiOlwiI2Y0YTQ2MFwiLFwic2VhZ3JlZW5cIjpcIiMyZThiNTdcIixcInNlYXNoZWxsXCI6XCIjZmZmNWVlXCIsXCJzaWVubmFcIjpcIiNhMDUyMmRcIixcInNreWJsdWVcIjpcIiM4N2NlZWJcIixcInNsYXRlYmx1ZVwiOlwiIzZhNWFjZFwiLFwic2xhdGVncmF5XCI6XCIjNzA4MDkwXCIsXCJzbGF0ZWdyZXlcIjpcIiM3MDgwOTBcIixcInNub3dcIjpcIiNmZmZhZmFcIixcInNwcmluZ2dyZWVuXCI6XCIjMDBmZjdmXCIsXCJzdGVlbGJsdWVcIjpcIiM0NjgyYjRcIixcInRhblwiOlwiI2QyYjQ4Y1wiLFwidGhpc3RsZVwiOlwiI2Q4YmZkOFwiLFwidG9tYXRvXCI6XCIjZmY2MzQ3XCIsXCJ0cmFuc3BhcmVudFwiOlwiIzAwMDBcIixcInR1cnF1b2lzZVwiOlwiIzQwZTBkMFwiLFwidmlvbGV0XCI6XCIjZWU4MmVlXCIsXCJ3aGVhdFwiOlwiI2Y1ZGViM1wiLFwid2hpdGVzbW9rZVwiOlwiI2Y1ZjVmNVwiLFwieWVsbG93Z3JlZW5cIjpcIiM5YWNkMzJcIixcInJlYmVjY2FwdXJwbGVcIjpcIiM2NjMzOTlcIn0iLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJAYmFiZWwvcnVudGltZS9oZWxwZXJzL2FzeW5jVG9HZW5lcmF0b3JcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcIkBiYWJlbC9ydW50aW1lL2hlbHBlcnMvaW50ZXJvcFJlcXVpcmVEZWZhdWx0XCIpOzsiLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJAYmFiZWwvcnVudGltZS9yZWdlbmVyYXRvclwiKTs7IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiY2hhbGtcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcImZzXCIpOzsiLCJtb2R1bGUuZXhwb3J0cyA9IHJlcXVpcmUoXCJmcy9wcm9taXNlc1wiKTs7IiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwib3NcIik7OyJdLCJzb3VyY2VSb290IjoiIn0=