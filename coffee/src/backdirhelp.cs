import chalk from 'chalk'
#chalk = require('chalk')
import {file} from 'mcktools/cmd'
import { spawn,exec } from 'child_process'
import util from 'util'
import pa from 'path'

px = util.promisify exec

cmdpath= pa.resolve(__dirname, '../../../bin')

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
	{ stdout, stderr } = await px "sudo #{cmdpath}/apfs-inspect /dev/#{disk}"
	console.log chalk.yellowBright "处理命令: info"
	log { stdout, stderr }

# 解析字符串, 拿到{filelist, dirlist}
str2list= (s) ->
	filelist=[]
	dirlist=[]
	reg= /^- INODE$([\s\S]*)^END: All done\.$/gm
	arr = [(s.matchAll reg)...]
	if arr.length!=1
		console.log chalk.redBright "出错了, list匹配结果数量不对#{arr.length} \n 匹配数组: #{arr}"
		console.log chalk.yellowBright "出错的原始输入: #{s}"
		return {filelist:[], dirlist:[]}
	ss= arr[0][1]
	# 解析单行
	#- DIR REC || Dirctry || target ID =     0x19 || name = .fseventsd
	#- DIR REC || Dirctry || target ID = 0x27d743 || name = .DocumentRevisions-V100
	#- DIR REC || RegFile || target ID = 0x234dac || name = tmbootpicker.efi
	#- DIR REC || Dirctry || target ID = 0x20000003f || name = 音乐
	# 需要排除的文件, 目录
	#- DIR REC || RegFile || target ID =     0x8b || name = .DS_Store
	#- DIR REC || Dirctry || target ID = 0x879d0b || name = $RECYCLE.BIN

	# 文件
	#- DIR REC || RegFile || target ID =     0xa4 || name = 地图-National Map.jp2
	regf= /^- DIR REC \|\| RegFile \|\| target ID =[ x0-9a-f]+\|\| name = ([^.$][^\n\r]*)$/gm
	f=  [(ss.matchAll regf)...]
	filelist.push x[1] for x in f

	# 目录
  #- DIR REC || Dirctry || target ID =     0x8c || name = ghost in the shell
	regr= /^- DIR REC \|\| Dirctry \|\| target ID =[ x0-9a-f]+\|\| name = ([^.$][^\n\r]*)$/gm
	r=  [(ss.matchAll regr)...]
	dirlist.push x[1] for x in r

	{filelist, dirlist}

# 得到文件和目录列表
getlist = ({disk, dir})->
	try
		{ stdout, stderr } = await px "sudo #{cmdpath}/apfs-list /dev/#{disk} 0 \"#{dir}\""
		{filelist, dirlist} = str2list stdout+stderr
		console.log chalk.yellowBright "扫描目录 : #{chalk.cyanBright dir} >>>>>>> 文件备份中: #{chalk.cyanBright filelist} "
		return {filelist, dirlist}
	catch err
		console.log chalk.redBright "list error:#{dir}"
	return {filelist:[], dirlist:[]}

# 处理列表命令, 列出目录下所有内容, 包含文件和文件夹: bd --list
export list = ({disk, dir})->
	console.log chalk.yellowBright "处理命令: list"
	{filelist, dirlist} = await getlist {disk, dir}
	log { stdout:filelist, stderr:dirlist}
# 备份单独一个文件的命令, bd --back
export back = ({disk, frompath, topath})->
	# 判断目录是否存在, 如果不存在就新建目录
	await file.newdir topath
	# 组装命令
	cmd="sudo #{cmdpath}/apfs-recover /dev/#{disk} 0 \"#{frompath}\" > \"#{topath}\""
	#console.log chalk.yellowBright "--------命令行-------",cmd
	try
		{ stdout, stderr } = await px cmd
	catch err
		console.log chalk.redBright "copy error:#{frompath}"
  
	#console.log chalk.yellowBright "copy file : #{chalk.cyanBright frompath} >>>>>>> #{chalk.cyanBright topath}"
	#log { stdout, stderr }



# 处理目录
bdir = ({disk, frompath, topath})->
	{filelist, dirlist} = await getlist {disk, dir:frompath}
	# todo 如果path=/ 那么下面的语句会导致 //xxx这种, 如果用if判断又比较丑陋, 没想好咋搞.
	await back {disk, frompath:"#{frompath}/#{x}", topath:"#{topath}/#{x}"} for x in filelist
	await bdir {disk, frompath:"#{frompath}/#{x}", topath:"#{topath}/#{x}"} for x in dirlist

		

# 默认的处理目录备份的接口
export bd = ({disk, frompath, topath})->
	# 判断目录是否存在, 如果不存在就新建目录
	await file.newdir topath
	await bdir {disk, frompath, topath}

	console.log chalk.yellowBright "backfinish, you can check error, that write in red word"

	
	#log { stdout, stderr }
export default {
	
	disklist
	afs
	info
	list
	back
	bd
}

