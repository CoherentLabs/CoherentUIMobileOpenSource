#!/usr/bin/env python

import os
import optparse

if __name__ == '__main__':
	parser = optparse.OptionParser()

	parser.add_option('', '--base', dest = 'basedir',
		help = 'path to root repository dir' , default = '../../../')

	(options, args) = parser.parse_args()

	mobileDotNetDir = os.path.join(options.basedir, 'Src/DotNet')

	iOSDir = os.path.join(mobileDotNetDir, 'swig/iOS')
	fileList = [ f for f in os.listdir(iOSDir) if os.path.isfile(os.path.join(iOSDir, f)) and f.endswith('cs') ]

	replacements = [
		['HandleRef jarg', 'IntPtr jarg'],
		['private HandleRef swigCPtr;', 'private IntPtr swigCPtr;'],
		['swigCPtr = new HandleRef(this, cPtr);', 'swigCPtr = cPtr;'],
		['new HandleRef(null, IntPtr.Zero)','IntPtr.Zero'],
		['internal static HandleRef getCPtr', 'internal static IntPtr getCPtr'],
		['if (swigCPtr.Handle != IntPtr.Zero)', 'if (swigCPtr != IntPtr.Zero)'],
		['Instances[swigCPtr.Handle]', 'Instances[swigCPtr]'],
	]

	for fileName in fileList:
		with open(os.path.join(iOSDir, fileName), 'r') as fin:
			contents = fin.read()
			for replacementPair in replacements:
				contents = contents.replace(replacementPair[0],
											replacementPair[1])

		with open(os.path.join(iOSDir, fileName), 'w') as fout:
			fout.write(contents)

