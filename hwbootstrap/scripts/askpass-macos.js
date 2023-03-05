#!/usr/bin/env osascript -l JavaScript

// This script originated from unetbootin askpass.js
// https://github.com/unetbootin/unetbootin/blob/master/src/unetbootin/unetbootin.app/Contents/Resources/askpass.js
// 
// License: GPL2-or-later

ObjC.import('stdlib')

const app = Application.currentApplication()
app.includeStandardAdditions = true

const result = app.displayDialog('Setup Assistant needs permissions to stage wireless firmware files from macOS for Hollywood system installation. Type your password to allow this.', {
  defaultAnswer: '',
  withIcon: 'stop',
  buttons: ['Cancel', 'Ok'],
  defaultButton: 'Ok',
  hiddenAnswer: true,
})

if (result.buttonReturned === 'Ok') {
  result.textReturned
} else {
  $.exit(255)
}