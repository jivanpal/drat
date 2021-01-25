(function() {
  var CleanWebpackPlugin, glob, nodeExternals, path, webpack;

  ({CleanWebpackPlugin} = require('clean-webpack-plugin'));

  nodeExternals = require('webpack-node-externals');

  glob = require("glob");

  path = require('path');

  webpack = require('webpack');

  //先按照测试环境配置, 生产环境是不一样的.
  module.exports = {
    mode: 'development',
    devtool: 'inline-source-map',
    target: 'node',
    output: {
      path: path.resolve(__dirname, 'dist/api'),
      //filename: '[name].[contenthash].js' # 这个会导致vscode的调试不生效. 好神奇.
      filename: '[name].js',
      library: 'backdir', // 指定library的name
      libraryTarget: 'umd' // 指定library编译的兼容性, common, es6, amd, umd, link...
    },
    //publicPath: '.'
    externals: [nodeExternals(), /^library\/.+$/], //这个很有用, 排除所有node_modules
    entry: {
      bin: './src/backdir.cs',
      ...(glob.sync('./src/**.cs').reduce(function(obj, el) {
        obj[path.parse(el).name] = el;
        return obj;
      }, {}))
    },
    plugins: [
      new CleanWebpackPlugin({
        cleanStaleWebpackAssets: false
      }),
      new webpack.BannerPlugin({
        banner: '#!/usr/bin/env node',
        raw: true
      })
    ],
    module: {
      rules: [
        {
          test: /\.m?js$/,
          exclude: /(node_modules|bower_components)/,
          use: {
            loader: 'babel-loader'
          }
        },
        {
          test: /\.coffee$|\.cs$/,
          exclude: /(node_modules|bower_components)/,
          loader: 'coffee-loader',
          options: {
            transpile: {
              presets: ['@babel/env',
        "@babel/react"]
            }
          }
        },
        {
          test: /\.cson$/,
          use: {
            loader: 'cson-loader'
          }
        }
      ]
    },
    optimization: {
      runtimeChunk: 'single',
      splitChunks: {
        cacheGroups: {
          vendor: {
            test: /[\\\/]node_modules[\\\/]/,
            name: 'vendors',
            chunks: 'all'
          }
        }
      }
    }
  };

  //resolve: # 解决自动查找index.cs而不是index.js的问题 #lib的配置
//	alias: mlib: path.resolve __dirname, '/Users/bergman/git/_X/code/lib/mcktools/src/'
//	extensions: ['.cs', '.coffee', '.mjs', '.js']

}).call(this);


//# sourceMappingURL=webpack.config.js.map
//# sourceURL=coffeescript