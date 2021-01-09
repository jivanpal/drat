import chalk from 'chalk'
#chalk = require('chalk')
import {cmd} from 'mlib'
import bdh from './backdirhelp.cs'


{disklist, afs,info,list,back}=bdh


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
	if cmd.iscmd x then cmds.push x else files.push x


console.log chalk.blueBright "命令:  #{cmds} "
console.log chalk.yellowBright "文件: #{files} "


# 处理命令
isdefault = cmds.length ==0
for x in cmds
	switch x
		when '--disklist' # 列出系统挂载的disk
			disklist 0
		when '--afs' # 列出系统挂载的disk
			afs files[0]
		when '--info' # 列出系统挂载的disk
			info files[0]
		when '--list' # 列出系统挂载的disk
			list disk:files[0], dir:files[1]
		when '--back' # 列出系统挂载的disk
		# 命令行的引号不重要
		#bd --back disk4 /地图/元素周期表-timg-3.jpeg /Volumes/wd4black/back/bbb.jpeg
		#bd --back disk4 "/地图/元素周期表-timg-3.jpeg" "/Volumes/wd4black/back/aaa.jpeg"
			back disk:files[0], frompath:files[1],topath:files[2]
		else console.log chalk.redBright '抱歉啊, 不知道你想干啥:', x
			

#	isdisklist=isdisklist ||/^--disklist$/.test x
#	isafs=isafs||/^--afs$/.test x

#isnothing= !isdefault && !isdisklist && !isafs

# 根据命令, 处理文件, 忽略node_module, 未来根据.gitignore忽略
if isdefault # 默认是跑测试文件
	console.log '默认行为:', chalk.cyanBright '啥也不干'
#if isdisklist # 列出系统挂载的disk
#	disklist 0
#if isafs # 列出系统挂载的disk
#	afs 0

#if isnothing
#	console.log chalk.redBright '抱歉啊: 不知道你想干啥'



