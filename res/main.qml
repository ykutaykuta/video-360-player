import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import "./item"
import CustomType 1.0

Window {
	id: frmMain
	visibility: Window.Maximized
	//    visibility: Window.FullScreen
	visible: true
	property bool isPlaying: true
	property bool isMute: false
	property int volume: 70 //max 1, min 0, step 0.1
	property bool isFullScreen: false

	Rectangle {
		id: rectViewArea
		anchors.fill: parent
		color: "transparent"

		OpenGLQML {
			id: opengl
			SequentialAnimation on t {
				NumberAnimation {
					to: 180
					duration: 10000
					easing.type: Easing.Linear
				}
				NumberAnimation {
					to: 0
					duration: 10000
					easing.type: Easing.Linear
				}
				loops: Animation.Infinite
				running: true
			}
			anchors.fill: parent
		}

		MouseArea {
			id: maControlArea
			width: parent.width
			height: parent.height / 11
			hoverEnabled: true
			anchors {
				left: parent.left
				bottom: parent.bottom
			}

			onEntered: {
				rectControlArea.visible = true
			}

			onExited: {
				rectControlArea.visible = false
			}

			Rectangle {
				id: rectControlArea
				anchors.fill: parent
				color: "transparent"

				Button {
					id: btnPlayPause
					height: parent.height / 2
					width: height
					anchors {
						left: parent.left
						leftMargin: width
						verticalCenter: parent.verticalCenter
					}
					Component.onCompleted: __behavior.cursorShape = Qt.PointingHandCursor
					style: ButtonStyle {
						background: Image {
							source: isPlaying ? "qrc:/res/image/pause.png" : "qrc:/res/image/play.png"
						}
					}
					onClicked: {
						if (isPlaying)
							Controller.pauseVideo()
						else
							Controller.playVideo()
						isPlaying = !isPlaying
					}
				}


				Button {
					id: btnVolume
					height: parent.height / 2
					width: height
					anchors {
						left: btnPlayPause.right
						leftMargin: width
						verticalCenter: btnPlayPause.verticalCenter
					}
					Component.onCompleted: __behavior.cursorShape = Qt.PointingHandCursor
					style: ButtonStyle {
						background: Image {
							source: isMute ? "qrc:/res/image/volume_mute.png" : volume
											 > 64 ? "qrc:/res/image/volume_high.png" : "qrc:/res/image/volume_low.png"
						}
					}
					onClicked: {
						isMute = !isMute
						if(isMute) {
							Controller.setVolume(0)
						}else {
							Controller.setVolume(volume)
						}
					}
				}

				CustomProgressBar {
					id: progressVolume
					width: btnVolume.width * 2
					height: btnVolume.height / 3
					minimum: 0
					maximum: 128
					value: 70
					anchors {
						verticalCenter: btnVolume.verticalCenter
						left: btnVolume.right
						leftMargin: btnVolume.width / 2
					}
					onValueChanged: {
						volume = value
						Controller.setVolume(volume)
						isMute = volume == 0 ? true:false
					}
				}

				Button {
					id: btnScreenMode
					width: btnVolume.width
					height: width
					anchors {
						verticalCenter: btnVolume.verticalCenter
						right: parent.right
						rightMargin: width
					}
					style: ButtonStyle {
						background: Image {
							source: isFullScreen ? "qrc:/res/image/normal_screen.png" : "qrc:/res/image/full_screen.png"
						}
					}
					Component.onCompleted: __behavior.cursorShape = Qt.PointingHandCursor
					onClicked: {
						isFullScreen = !isFullScreen
						frmMain.visibility = isFullScreen ? Window.FullScreen : Window.Maximized
					}
				}

				Button {
					id: btnOpenFile
					width: btnVolume.width
					height: width
					anchors {
						verticalCenter: btnVolume.verticalCenter
						right: btnScreenMode.left
						rightMargin: width
					}
					style: ButtonStyle {
						background: Image {
							source: "qrc:/res/image/open_file.png"
						}
					}
					Component.onCompleted: __behavior.cursorShape = Qt.PointingHandCursor
					onClicked: {
						Controller.openFileDialog()
					}
				}

				CustomProgressBar {
					id: progressVideo
					height: progressVolume.height
					width: parent.width - height * 4
					maximum: Controller.Duration
					value: Controller.CurrTime
					colorBackground: "white"
					colorPanel: "red"
					colorProgress: colorPanel
					anchors {
						horizontalCenter: parent.horizontalCenter
						top: parent.top
					}

					onValueChanged: {
						if (isOutsideControl){
							Controller.seekVideo(value)
						}
					}
				}

				Connections {
					id: connectProgressVideo
					target: Controller
					onCurrTimeChanged: {
						if (progressVideo.isOutsideControl)
							return
						progressVideo.value = Controller.CurrTime
					}
				}
				Text {
					id: txtCurrentTimeVideo
					anchors {
						verticalCenter: progressVolume.verticalCenter
						left: progressVolume.right
						leftMargin: progressVolume.height * 2
					}
					color: "white"
					font.bold: true
					font.pointSize: 13
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					text: formatTime(progressVideo.value)
				}

				Text {
					id: txtDurationVideo
					anchors {
						verticalCenter: txtCurrentTimeVideo.verticalCenter
						left: txtCurrentTimeVideo.right
					}
					color: txtCurrentTimeVideo.color
					font.bold: true
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					font.pointSize: txtCurrentTimeVideo.font.pointSize
					text: " / " + formatTime(progressVideo.maximum)
				}
			}
		}
	}

	function formatTime(timeInSeconds) {
		var minutes = Math.floor(timeInSeconds / 60 % 60)
		var hours = Math.floor(timeInSeconds / 3600)
		var seconds = timeInSeconds - minutes * 60 - hours * 60
		if (seconds < 10)
			seconds = "0" + seconds
		if (minutes < 10)
			minutes = "0" + minutes
		return hours + ":" + minutes + ":" + seconds
	}
}
