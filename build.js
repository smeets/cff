var fs = require('fs')
var exec = require('child_process').exec
var path = require('path')

function recurseFolder(root, dir, file_fn) {
	fs.readdir(path.join(root, dir), function (err, entries) {
		if (err) throw err
		entries.forEach(function (entry) {
			fs.stat(path.join(root, dir, entry), function (err, stat) {
				if (stat.isDirectory())
					return recurseFolder(root, path.join(dir, entry), file_fn)

				if (stat.isFile())
					return file_fn(dir, entry)
			})
		})
	})
}

recurseFolder('./res/', 'intro', dot_to_svg)

function dot_to_svg(dir, file) {
	var cmd = 'dot'
	cmd += ' -o' + path.join('./bin/', dir, path.basename(file, '.dot') + '.svg')
	cmd += ' -Tsvg'
	cmd += ' ' + path.join('./res', dir, file)
	console.log('dot_2_svg', cmd)
	exec(cmd, function (err, stdout, stderr) {
		if (err) throw err
		console.error(stderr)
	})
}