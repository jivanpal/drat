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
exports.id = "top";
exports.ids = ["top"];
exports.modules = {

/***/ "./src/top.cs":
/*!********************!*
  !*** ./src/top.cs ***!
  \********************/
/***/ ((__unused_webpack_module, exports, __webpack_require__) => {

"use strict";


var _interopRequireDefault = __webpack_require__(/*! @babel/runtime/helpers/interopRequireDefault */ "@babel/runtime/helpers/interopRequireDefault");

Object.defineProperty(exports, "__esModule", ({
  value: true
}));
exports.top1 = void 0;

var _toConsumableArray2 = _interopRequireDefault(__webpack_require__(/*! @babel/runtime/helpers/toConsumableArray */ "@babel/runtime/helpers/toConsumableArray"));

/*
 * 这里是寻找某个位置的点的函数, 比如中位数, 1/4位数等等
 * @param {输入的用来寻找的数组} arra
 * @param {每个元素拿值的方法} valu
 * @param {要找到顶部的第几个元素} top
 * 这个算法类似于快速排序. 每次只排一边, 知道找到需要的值.
 * 最终返回的是满足条件的值.
 * 目标是统计数据的提取, 比如中位数, 3/4位数等等.

export function top(arra, valu, top) {
	if (arra.length === 0) return [];
	const lesser = [];
	const greater = [];
	const p = arra[0];
	for (let i = 1; i < arra.length; i++) {
		if (valu(arra[i]) > valu(p)) {
			greater.push(arra[i]);
		} else {
			lesser.push(arra[i]);
		}
	}

	// 终结条件很重要, p是第top个, 或者less==top个, 其实都可以返回了.这样就是正好top个或者多一个.
	if (top - 2 < lesser.length && lesser.length < top + 1) return p;
	if (lesser.length > top) {
		return top(lesser, valu, top);
	}
	return top(greater, valu, top - lesser.length - 1); // 这里要把p的位置减掉, 不然就bug了.
}
 */
var big;

big = function big(a, b) {
  return a > b; //默认这个是找最大的数字.
};

var top1 = function top1(_ref) {
  var arr = _ref.arr,
      _ref$xwiny = _ref.xwiny,
      xwiny = _ref$xwiny === void 0 ? big : _ref$xwiny;
  var a, e, i, j, len;
  a = (0, _toConsumableArray2["default"])(arr); // 不改变输入的纯函数

  e = a.shift(0);

  for (i = 0, len = a.length; i < len; i++) {
    j = a[i];

    if (xwiny(j, e)) {
      e = j;
    }
  }

  return e;
}; // 另一个纯函数, 但是重复判断了第一个元素, 不过很可能效率更好
//e=arr[0]
//	e =j for j in arr when xwiny j,e


exports.top1 = top1;

/***/ }),

/***/ "@babel/runtime/helpers/interopRequireDefault":
/*!***************************************************************!*
  !*** external "@babel/runtime/helpers/interopRequireDefault" ***!
  \***************************************************************/
/***/ ((module) => {

"use strict";
module.exports = require("@babel/runtime/helpers/interopRequireDefault");;

/***/ }),

/***/ "@babel/runtime/helpers/toConsumableArray":
/*!***********************************************************!*
  !*** external "@babel/runtime/helpers/toConsumableArray" ***!
  \***********************************************************/
/***/ ((module) => {

"use strict";
module.exports = require("@babel/runtime/helpers/toConsumableArray");;

/***/ })

};
;

// load runtime
var __webpack_require__ = require("../runtime/runtime.js");
__webpack_require__.C(exports);
return __webpack_require__.X([], "./src/top.cs");
})();
});
//# sourceMappingURL=data:application/json;charset=utf-8;base64,eyJ2ZXJzaW9uIjozLCJzb3VyY2VzIjpbIndlYnBhY2s6Ly9tbGliL3dlYnBhY2svdW5pdmVyc2FsTW9kdWxlRGVmaW5pdGlvbiIsIndlYnBhY2s6Ly9tbGliLy4vc3JjL3RvcC5jcyIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy9pbnRlcm9wUmVxdWlyZURlZmF1bHRcIiIsIndlYnBhY2s6Ly9tbGliL2V4dGVybmFsIFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy90b0NvbnN1bWFibGVBcnJheVwiIl0sIm5hbWVzIjpbXSwibWFwcGluZ3MiOiI7QUFBQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQTtBQUNBO0FBQ0E7QUFDQSxDQUFDO0FBQ0QsTzs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7O0FDc0JHOzs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7Ozs7QUFBQTs7QUFFSCxNQUFLO1NBQVUsSUFBVixDLENBQUE7QUFBQSxDQUFMOztBQUNBLElBQU8sT0FBTyxTQUFQLElBQU87QUFBQSxNQUFDLEdBQUQsUUFBQyxHQUFEO0FBQUEsd0JBQU0sS0FBTjtBQUFBLE1BQU0sS0FBTjtBQUNkO0FBQUMsMENBQUQsR0FBQyxFQURhLENBQ2Q7O0FBQ0MsTUFBRSxDQUFDLENBQUQsUUFBRjs7QUFDQTs7O1FBQXNCLFcsRUFBQTtBQUF0QixVQUFJLENBQUo7O0FBQUE7O1NBQ0EsQztBQUphLENBQWQsQyxDQUhHOzs7Ozs7Ozs7Ozs7Ozs7O0FDaENILDBFOzs7Ozs7Ozs7OztBQ0FBLHNFIiwiZmlsZSI6InRvcC90b3AuanMiLCJzb3VyY2VzQ29udGVudCI6WyIoZnVuY3Rpb24gd2VicGFja1VuaXZlcnNhbE1vZHVsZURlZmluaXRpb24ocm9vdCwgZmFjdG9yeSkge1xuXHRpZih0eXBlb2YgZXhwb3J0cyA9PT0gJ29iamVjdCcgJiYgdHlwZW9mIG1vZHVsZSA9PT0gJ29iamVjdCcpXG5cdFx0bW9kdWxlLmV4cG9ydHMgPSBmYWN0b3J5KCk7XG5cdGVsc2UgaWYodHlwZW9mIGRlZmluZSA9PT0gJ2Z1bmN0aW9uJyAmJiBkZWZpbmUuYW1kKVxuXHRcdGRlZmluZShbXSwgZmFjdG9yeSk7XG5cdGVsc2UgaWYodHlwZW9mIGV4cG9ydHMgPT09ICdvYmplY3QnKVxuXHRcdGV4cG9ydHNbXCJtbGliXCJdID0gZmFjdG9yeSgpO1xuXHRlbHNlXG5cdFx0cm9vdFtcIm1saWJcIl0gPSBmYWN0b3J5KCk7XG59KShnbG9iYWwsIGZ1bmN0aW9uKCkge1xucmV0dXJuICIsIlxuXG4jIyNcbiAqIOi/memHjOaYr+Wvu+aJvuafkOS4quS9jee9rueahOeCueeahOWHveaVsCwg5q+U5aaC5Lit5L2N5pWwLCAxLzTkvY3mlbDnrYnnrYlcbiAqIEBwYXJhbSB76L6T5YWl55qE55So5p2l5a+75om+55qE5pWw57uEfSBhcnJhXG4gKiBAcGFyYW0ge+avj+S4quWFg+e0oOaLv+WAvOeahOaWueazlX0gdmFsdVxuICogQHBhcmFtIHvopoHmib7liLDpobbpg6jnmoTnrKzlh6DkuKrlhYPntKB9IHRvcFxuICog6L+Z5Liq566X5rOV57G75Ly85LqO5b+r6YCf5o6S5bqPLiDmr4/mrKHlj6rmjpLkuIDovrksIOefpemBk+aJvuWIsOmcgOimgeeahOWAvC5cbiAqIOacgOe7iOi/lOWbnueahOaYr+a7oei2s+adoeS7tueahOWAvC5cbiAqIOebruagh+aYr+e7n+iuoeaVsOaNrueahOaPkOWPliwg5q+U5aaC5Lit5L2N5pWwLCAzLzTkvY3mlbDnrYnnrYkuXG4gXG5cbmV4cG9ydCBmdW5jdGlvbiB0b3AoYXJyYSwgdmFsdSwgdG9wKSB7XG5cdGlmIChhcnJhLmxlbmd0aCA9PT0gMCkgcmV0dXJuIFtdO1xuXHRjb25zdCBsZXNzZXIgPSBbXTtcblx0Y29uc3QgZ3JlYXRlciA9IFtdO1xuXHRjb25zdCBwID0gYXJyYVswXTtcblx0Zm9yIChsZXQgaSA9IDE7IGkgPCBhcnJhLmxlbmd0aDsgaSsrKSB7XG5cdFx0aWYgKHZhbHUoYXJyYVtpXSkgPiB2YWx1KHApKSB7XG5cdFx0XHRncmVhdGVyLnB1c2goYXJyYVtpXSk7XG5cdFx0fSBlbHNlIHtcblx0XHRcdGxlc3Nlci5wdXNoKGFycmFbaV0pO1xuXHRcdH1cblx0fVxuXG5cdC8vIOe7iOe7k+adoeS7tuW+iOmHjeimgSwgcOaYr+esrHRvcOS4qiwg5oiW6ICFbGVzcz09dG9w5LiqLCDlhbblrp7pg73lj6/ku6Xov5Tlm57kuoYu6L+Z5qC35bCx5piv5q2j5aW9dG9w5Liq5oiW6ICF5aSa5LiA5LiqLlxuXHRpZiAodG9wIC0gMiA8IGxlc3Nlci5sZW5ndGggJiYgbGVzc2VyLmxlbmd0aCA8IHRvcCArIDEpIHJldHVybiBwO1xuXHRpZiAobGVzc2VyLmxlbmd0aCA+IHRvcCkge1xuXHRcdHJldHVybiB0b3AobGVzc2VyLCB2YWx1LCB0b3ApO1xuXHR9XG5cdHJldHVybiB0b3AoZ3JlYXRlciwgdmFsdSwgdG9wIC0gbGVzc2VyLmxlbmd0aCAtIDEpOyAvLyDov5nph4zopoHmiopw55qE5L2N572u5YeP5o6JLCDkuI3nhLblsLFidWfkuoYuXG59XG4jIyNcblxuYmlnPSAoYSwgYikgLT4gYT5iICPpu5jorqTov5nkuKrmmK/mib7mnIDlpKfnmoTmlbDlrZcuXG5leHBvcnQgdG9wMSA9ICh7YXJyLHh3aW55PWJpZ30pLT5cblx0YT1bYXJyLi4uXSAjIOS4jeaUueWPmOi+k+WFpeeahOe6r+WHveaVsFxuXHRlPWEuc2hpZnQgMFxuXHRlID0gaiBmb3IgaiBpbiBhIHdoZW4geHdpbnkgaixlXG5cdGVcblx0IyDlj6bkuIDkuKrnuq/lh73mlbAsIOS9huaYr+mHjeWkjeWIpOaWreS6huesrOS4gOS4quWFg+e0oCwg5LiN6L+H5b6I5Y+v6IO95pWI546H5pu05aW9XG5cdCNlPWFyclswXVxuXHQjXHRlID1qIGZvciBqIGluIGFyciB3aGVuIHh3aW55IGosZVxuIiwibW9kdWxlLmV4cG9ydHMgPSByZXF1aXJlKFwiQGJhYmVsL3J1bnRpbWUvaGVscGVycy9pbnRlcm9wUmVxdWlyZURlZmF1bHRcIik7OyIsIm1vZHVsZS5leHBvcnRzID0gcmVxdWlyZShcIkBiYWJlbC9ydW50aW1lL2hlbHBlcnMvdG9Db25zdW1hYmxlQXJyYXlcIik7OyJdLCJzb3VyY2VSb290IjoiIn0=