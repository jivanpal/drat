module.exports = {
	extends: [
		"eslint:recommended",
		"plugin:react/recommended",
		'wind/node'],
	globals: {
		FileReader: false,
		DOMException: false,
		Headers: false,
		fetch: false,
	},
	rules: {
		'strict': [0],
		'vars-on-top': 0,
		'global-require': 0,
		"indent": 0,
		"no-tabs":0,
		"prettier/prettier": 0,
		"spaced-comment":0,
		'new-cap':0,
	},
	parser: 'babel-eslint',
	parserOptions: {
		sourceType: 'module',
		allowImportExportEverywhere: true,
	}
};
