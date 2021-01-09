import chalk from 'chalk'
#chalk = require('chalk')
import {file} from 'mlib'
import { spawn,exec } from 'child_process'
import util from 'util'

px = util.promisify exec



# -----------------pure function-----------------
# 输出
log= ({ stdout, stderr })->
	console.log 'stdout:', chalk.greenBright stdout
	console.error 'stderr:', chalk.redBright stderr
# -----------------export function-----------------


# 处理disklist命令: bd --disklist
export disklist = ->
	{ stdout, stderr } = await px 'diskutil list'
	console.log chalk.yellowBright "处理命令: disklist"
	log { stdout, stderr }


# 处理afs命令: bd --afs
export afs = (disk)->
	{ stdout, stderr } = await px "sudo fsck_apfs /dev/#{disk}"
	console.log chalk.yellowBright "处理命令: afs"
	log { stdout, stderr }


# 处理info命令: bd --info
export info = (disk)->
	{ stdout, stderr } = await px "sudo ../bin/apfs-inspect /dev/#{disk}"
	console.log chalk.yellowBright "处理命令: info"
	log { stdout, stderr }

# 处理列表命令, 列出目录下所有内容, 包含文件和文件夹: bd --list
export list = ({disk, dir})->
	{ stdout, stderr } = await px "sudo ../bin/apfs-list /dev/#{disk} 0 #{dir}"
	console.log chalk.yellowBright "处理命令: list"
	log { stdout, stderr }

# 备份单独一个文件的命令, bd --back

export back = ({disk, frompath, topath})->
	# 判断目录是否存在, 如果不存在就新建目录
	await file.newdir topath
	# 组装命令
	cmd="sudo ../bin/apfs-recover /dev/#{disk} 0 \"#{frompath}\" > \"#{topath}\""
	console.log chalk.yellowBright "--------命令行-------",cmd
	{ stdout, stderr } = await px cmd

	console.log chalk.yellowBright "处理命令: back"
	#log { stdout, stderr }

export default {
	
	disklist
	afs
	info
	list
	back
}

