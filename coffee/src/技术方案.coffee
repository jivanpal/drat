import chalk from 'chalk'
#chalk = require('chalk')
#import {file} from 'mlib'
import lph from './lornpughelp.cs'
import { spawn,exec } from 'child_process'
import util from 'util'

{iscmd, dolp, dodir}=lph


# ------------------init---------------------
arg=process.argv[2..]
console.log '环境和参数信息:'
console.table [
	{'键: ':'process.cwd()', '值: ': process.cwd()}
	{'键: ':'__dirname', '值: ':__dirname}
	{'键: ':'process.argv', '值: ':process.argv}
	{'键: ':'真参数', '值: ':arg}
]

cmds=[] # 传入的参数的命令数组
files=[] # 传入的文件数组

for x in arg
	if iscmd x then cmds.push x else files.push x


console.log chalk.blueBright "命令:  #{cmds} "
console.log chalk.yellowBright "文件: #{files} "


# 处理命令
isdefault = cmds.length ==0
for x in cmds
	isdir=/^--dir$/.test x
	isio=/^--io$/.test x
	islp=/^--lp$/.test x




# 根据命令, 处理文件, 忽略node_module, 未来根据.gitignore忽略
if isdefault # 默认是跑测试文件
	console.log (chalk.cyanBright '默认行为:'), '测试结果: '

else if islp # 修改.zshrc文件, 增加快捷方式
	dolp 0
else if isdir # 建测试目录, 初始化测试文件
	if files.length==0
		dodir '.' #如果没有任何文件参数, 那么就是从本地搞深度遍历
	else
		dodir x for x in files
		
	console.log (chalk.cyanBright '初始化'), '初始化成功: '

else if isio # 增加测试文件
	console.log (chalk.cyanBright '初始化'), '初始化成功: '
	
else
	console.log chalk.redBright '抱歉啊: 不知道你想干啥'





# 调用外部命令
ls = spawn('ls', ['-lh', '/usr'])
ls.stdout.on 'data', (data) ->
	console.log chalk.yellowBright "spawn"
	console.log chalk.greenBright "stdout: #{data}"
ls.stderr.on 'data', (data) ->
	console.log chalk.yellowBright "spawn"
	console.error chalk.redBright "stderr: #{data}"
ls.on 'close', (code) ->
	console.log chalk.yellowBright "spawn"
	console.log chalk.yellowBright "child process exited with code #{code}"
# exec方式
exec 'cat *.js missing_file | wc -l', (error, stdout, stderr) ->
	if error
		console.log chalk.yellowBright "exec"
		console.error chalk.redBright "exec error: #{error}"
		return
	console.log chalk.yellowBright "exec"
	console.log chalk.greenBright "stdout: #{stdout}"
	console.error chalk.redBright "stderr: #{stderr}"

# promise的exec
px = util.promisify exec
do ->
	{ stdout, stderr } = await px 'ls -a .*'
	console.log chalk.yellowBright "execpromise"
	console.log 'stdout:', chalk.greenBright stdout
	console.error 'stderr:', chalk.redBright stderr



###
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
###
