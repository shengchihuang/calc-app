const { app, BrowserWindow } = require('electron')
var addonJSCalcEng = require('./build/Release/JSCalcEng.node')

function createWindow () {
	const win = new BrowserWindow({
		width: 800,
		height: 600,
		webPreferences: {
		nodeIntegration: true
	}
})
win.loadFile('index.html')
}
app.whenReady().then(createWindow)
app.on('window-all-closed', () => {
	if (process.platform !== 'darwin') {
		app.quit()
	}
})
app.on('activate', () => {
	if (BrowserWindow.getAllWindows().length === 0) {
		createWindow()
	}
})
console.log('Calculator:', addonJSCalcEng.JSCalcEng(123,234,345)) // 123+234*345=80853
console.log('Calculator:', addonJSCalcEng.JSCalcEng(333,222,111)) // 123+234*345=24975
