import chalk from 'chalk'
#chalk = require('chalk')
import {cmd} from 'mcktools/cmd'
import bdh from './backdirhelp.cs'


{disklist, afs,info,list,back, bd}=bdh
# ------------------init---------------------
do ->

	{cmds, files}=cmd.initcmd 0

	# 处理默认的备份目录命令
	#bd --back disk4 /地图 /Volumes/wd4black/back
	#bd --back disk4 "/地图" "/Volumes/wd4black/back"
	if cmds.length ==0 # 默认是跑测试文件
		console.log  chalk.cyanBright '进行目录备份'
		if files.length!=3
			console.log  chalk.redBright '参数数量错误', files.length, files
			return
		bd disk:files[0], frompath:files[1],topath:files[2]
		return 0

	# 处理命令
	for x in cmds
		switch x
			when '--disklist' # 列出系统挂载的disk: bd --disklist
				disklist 0
			when '--afs' # 列出某个disk的afs信息: bd --afs disk4
				afs files[0]
			when '--info' # 列出某个disk的详细afs信息: bd --info disk4
				info files[0]
			when '--list' # 列出某个disk的某个dir的详细信息: bd --list disk4 /
				list disk:files[0], dir:files[1]
			when '--back'
			# 备份某个单独的文件, 需要参数: disk, 要被备份的文件路径, 备份copy的目标路径. 命令行的引号不重要
			#bd --back disk4 /地图/元素周期表-timg-3.jpeg /Volumes/wd4black/back/bbb.jpeg
			#bd --back disk4 "/地图/元素周期表-timg-3.jpeg" "/Volumes/wd4black/back/aaa.jpeg"
				back disk:files[0], frompath:files[1],topath:files[2]
			else console.log chalk.redBright '抱歉啊, 不知道你想干啥:', x
