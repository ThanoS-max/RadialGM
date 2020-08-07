#include "SoundEditor.h"
#include "ui_SoundEditor.h"

#include "gmx.h"

#include "Components/ArtManager.h"
#include "Components/Utility.h"

#include <QDebug>
#include <QDesktopServices>
#include <QSlider>
#include <QTime>

using Sound = buffers::resources::Sound;

SoundEditor::SoundEditor(EditorModel* model, QWidget* parent)
    : BaseEditor(model, parent),
      _ui(new Ui::SoundEditor),
      _mediaPlayer(new QMediaPlayer(this)),
      _playlist(new QMediaPlaylist(_mediaPlayer)),
      _userPaused(false) {
  _ui->setupUi(this);

  _mapper->mapName(_ui->nameEdit);
  _mapper->pushResource();

  _mapper->mapField(Sound::kVolumeFieldNumber, _ui->volumeSpinBox);

  _mapper->load();

  _ui->volumeSlider->setValue(static_cast<int>(_ui->volumeSpinBox->value() * 100));

  connect(_ui->saveButton, &QAbstractButton::pressed, this, &BaseEditor::OnSave);

  _playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
  _mediaPlayer->setPlaylist(_playlist);
  // Update the signals every 50ms instead of Qt's default 1000ms to keep slider up to date
  _mediaPlayer->setNotifyInterval(50);

  connect(_mediaPlayer, &QMediaPlayer::positionChanged, [=]() {
    if (_mediaPlayer->duration() > 0) {
      int percent = static_cast<int>(100 * (static_cast<float>(_mediaPlayer->position()) / _mediaPlayer->duration()));
      _ui->playbackSlider->setSliderPosition(percent);
    } else
      _ui->playbackSlider->setSliderPosition(0);

    QTime timestamp(0, 0);
    _ui->playbackPositionLabel->setText(timestamp.addMSecs(static_cast<int>(_mediaPlayer->position())).toString());
  });

  connect(_mediaPlayer, &QMediaPlayer::stateChanged, [=]() {
    if (_mediaPlayer->state() == QMediaPlayer::PausedState || _mediaPlayer->state() == QMediaPlayer::StoppedState)
      _ui->playButton->setIcon(ArtManager::GetIcon(":/actions/play.png"));
    else
      _ui->playButton->setIcon(ArtManager::GetIcon(":/actions/pause.png"));
  });

  _mediaPlayer->setPlaylist(_playlist);
}

SoundEditor::~SoundEditor() { delete _ui; }

void SoundEditor::on_playButton_clicked() {
  if (_mediaPlayer->state() == QMediaPlayer::PausedState || _mediaPlayer->state() == QMediaPlayer::StoppedState) {
    _mediaPlayer->play();
    _userPaused = false;
  } else {
    _mediaPlayer->pause();
    _userPaused = true;
  }
}

void SoundEditor::on_loopButton_clicked() {
  if (_playlist->playbackMode() == QMediaPlaylist::CurrentItemOnce)
    _playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
  else
    _playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
}

void SoundEditor::on_playbackSlider_sliderPressed() {
  if (_mediaPlayer->state() == QMediaPlayer::PlayingState) _mediaPlayer->pause();
}

void SoundEditor::on_playbackSlider_sliderReleased() {
  _mediaPlayer->setPosition(static_cast<int>((_ui->playbackSlider->value() / 100.f) * _mediaPlayer->duration()));
  if (_mediaPlayer->state() == QMediaPlayer::PausedState && !_userPaused) _mediaPlayer->play();
}

void SoundEditor::on_volumeSlider_sliderMoved(int position) {
  _ui->volumeSpinBox->setValue(position / 100.0);
  _mediaPlayer->setVolume(position);
}

void SoundEditor::on_volumeSpinBox_valueChanged(double arg1) {
  _ui->volumeSlider->setValue(static_cast<int>(arg1 * 100));
  _mediaPlayer->setVolume(static_cast<int>(arg1 * 100));
}

void SoundEditor::on_saveAsButton_clicked() {
  //  TODO: implement this when egm is done
}

void SoundEditor::on_loadButton_clicked() {

}

void SoundEditor::on_editButton_clicked() {

}

void SoundEditor::on_stopButton_clicked() { _mediaPlayer->stop(); }
