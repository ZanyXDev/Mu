#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QTimer>
#include <QTouchEvent>
#include <QMessageBox>
#include <QSettings>
#include <QFont>
#include <QIcon>
#include <QObject>
#include <QEvent>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QCoreApplication>
#include <QPixmap>
#include <QAudioOutput>
#include <QAudioFormat>

#include <stdint.h>

#include "debugviewer.h"
#include "settingsmanager.h"
#include "statemanager.h"


MainWindow::MainWindow(QWidget* parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow){
   QAudioFormat format;
   QString resourceDirPath;
   bool hideOnscreenKeys;

   //audio output
   format.setSampleRate(AUDIO_SAMPLE_RATE);
   format.setChannelCount(2);
   format.setSampleSize(16);
   format.setCodec("audio/pcm");
#if Q_BYTE_ORDER == Q_BIG_ENDIAN
   format.setByteOrder(QAudioFormat::BigEndian);
#else
   format.setByteOrder(QAudioFormat::LittleEndian);
#endif
   format.setSampleType(QAudioFormat::SignedInt);

   //submodules
   settings = new QSettings(QDir::homePath() + "/MuCfg.txt", QSettings::IniFormat);//settings is public, create it first
   settingsManager = new SettingsManager(this);
   stateManager = new StateManager(this);
   emuDebugger = new DebugViewer(this);
   refreshDisplay = new QTimer(this);
   audioDevice = new QAudioOutput(format, this);
   audioOut = audioDevice->start();

   //resource directory
   resourceDirPath = settings->value("resourceDirectory", "").toString();

   //get default path if path not set
   if(resourceDirPath == ""){
#if defined(Q_OS_ANDROID)
      resourceDirPath = "/sdcard/Mu";
#elif defined(Q_OS_IOS)
      resourceDirPath = "/var/mobile/Media/Mu";
#else
      resourceDirPath = QDir::homePath() + "/Mu";
#endif
      settings->setValue("resourceDirectory", resourceDirPath);
   }

   //create directory tree, in case someone deleted it since the emu was last run or it was never created
   createHomeDirectoryTree(resourceDirPath);

   //keyboard
   for(uint8_t index = 0; index < EmuWrapper::BUTTON_TOTAL_COUNT; index++)
      keyForButton[index] = settings->value("palmButton" + QString::number(index) + "Key", '\0').toInt();

   //GUI
   ui->setupUi(this);

   //check if onscreen keys should be hidden
   if(settings->value("hideOnscreenKeys", "").toString() == ""){
#if defined(Q_OS_ANDROID) && defined(Q_OS_IOS)
      settings->setValue("hideOnscreenKeys", false);
#else
      //not a mobile device disable the on screen buttons
      settings->setValue("hideOnscreenKeys", true);
#endif
   }
   hideOnscreenKeys = settings->value("hideOnscreenKeys", "").toBool();

   //this makes the display window and button icons resize properly
   ui->centralWidget->installEventFilter(this);
   ui->centralWidget->setObjectName("centralWidget");

   ui->up->installEventFilter(this);
   ui->down->installEventFilter(this);
   ui->left->installEventFilter(this);
   ui->right->installEventFilter(this);
   ui->center->installEventFilter(this);

   ui->calendar->installEventFilter(this);
   ui->addressBook->installEventFilter(this);
   ui->todo->installEventFilter(this);
   ui->notes->installEventFilter(this);

   ui->power->installEventFilter(this);

   ui->ctrlBtn->installEventFilter(this);
   ui->install->installEventFilter(this);
   ui->debugger->installEventFilter(this);
   ui->screenshot->installEventFilter(this);
   ui->stateManager->installEventFilter(this);
   ui->reset->installEventFilter(this);

   //hide onscreen keys if needed
   ui->up->setHidden(hideOnscreenKeys);
   ui->down->setHidden(hideOnscreenKeys);
   ui->left->setHidden(hideOnscreenKeys);
   ui->right->setHidden(hideOnscreenKeys);
   ui->center->setHidden(hideOnscreenKeys);

   ui->calendar->setHidden(hideOnscreenKeys);
   ui->addressBook->setHidden(hideOnscreenKeys);
   ui->todo->setHidden(hideOnscreenKeys);
   ui->notes->setHidden(hideOnscreenKeys);

   ui->power->setHidden(hideOnscreenKeys);

#if !defined(EMU_DEBUG) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
   //doesnt support debug tools
   ui->debugger->hide();
#endif
   connect(refreshDisplay, SIGNAL(timeout()), this, SLOT(updateDisplay()));
   refreshDisplay->start(1000 / EMU_FPS);//update display every X milliseconds
}

MainWindow::~MainWindow(){
   refreshDisplay->stop();
   delete stateManager;
   delete emuDebugger;
   delete refreshDisplay;
   delete audioDevice;
   delete settings;//settings is public, destroy it last
   delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event){
   int key = event->key();

   for(uint8_t index = 0; index < EmuWrapper::BUTTON_TOTAL_COUNT; index++)
      if(keyForButton[index] == key)
         emu.setKeyValue(index, true);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event){
   int key = event->key();

   for(uint8_t index = 0; index < EmuWrapper::BUTTON_TOTAL_COUNT; index++)
      if(keyForButton[index] == key)
         emu.setKeyValue(index, false);
}

void MainWindow::createHomeDirectoryTree(const QString& path){
   QDir homeDir(path);

   //creates directorys if not present, does nothing if they exist already
   homeDir.mkpath(".");
   homeDir.mkpath("./saveStates");
   homeDir.mkpath("./screenshots");
   homeDir.mkpath("./debugDumps");
}

uint32_t MainWindow::getEmuFeatureList(){
   uint32_t features = FEATURE_ACCURATE;

   features |= settings->value("feature128mbRam", false).toBool() ? FEATURE_RAM_HUGE : 0;
   features |= settings->value("featureFastCpu", false).toBool() ? FEATURE_FAST_CPU : 0;
   features |= settings->value("featureHybridCpu", false).toBool() ? FEATURE_HYBRID_CPU : 0;
   features |= settings->value("featureCustomFb", false).toBool() ? FEATURE_CUSTOM_FB : 0;
   features |= settings->value("featureSyncedRtc", false).toBool() ? FEATURE_SYNCED_RTC : 0;
   features |= settings->value("featureHleApis", false).toBool() ? FEATURE_HLE_APIS : 0;
   features |= settings->value("featureEmuHonest", false).toBool() ? FEATURE_EMU_HONEST : 0;
   features |= settings->value("featureExtraKeys", false).toBool() ? FEATURE_EXT_KEYS : 0;
   features |= settings->value("featureSoundStreams", false).toBool() ? FEATURE_SND_STRMS : 0;

#if defined(EMU_DEBUG)
   features |= FEATURE_DEBUG;
#endif

   //lazy debug overrides
   //features = FEATURE_EXT_KEYS | FEATURE_EMU_HONEST | FEATURE_FAST_CPU | FEATURE_HYBRID_CPU | FEATURE_CUSTOM_FB | FEATURE_DEBUG | FEATURE_SND_STRMS;
   //features = FEATURE_FAST_CPU | FEATURE_HYBRID_CPU | FEATURE_CUSTOM_FB | FEATURE_DEBUG;
   //features = FEATURE_FAST_CPU | FEATURE_HYBRID_CPU | FEATURE_DEBUG;

   return features;
}

void MainWindow::popupErrorDialog(const QString& error){
   QMessageBox::critical(this, "Mu", error, QMessageBox::Ok);
}

void MainWindow::popupInformationDialog(const QString& info){
   QMessageBox::information(this, "Mu", info, QMessageBox::Ok);
}

void MainWindow::redraw(){
   bool hideOnscreenKeys = settings->value("hideOnscreenKeys", "").toBool();
   QResizeEvent* resizeEvent = new QResizeEvent(ui->centralWidget->size(), ui->centralWidget->size());

   //update current keys
   ui->up->setHidden(hideOnscreenKeys);
   ui->down->setHidden(hideOnscreenKeys);
   ui->left->setHidden(hideOnscreenKeys);
   ui->right->setHidden(hideOnscreenKeys);
   ui->center->setHidden(hideOnscreenKeys);

   ui->calendar->setHidden(hideOnscreenKeys);
   ui->addressBook->setHidden(hideOnscreenKeys);
   ui->todo->setHidden(hideOnscreenKeys);
   ui->notes->setHidden(hideOnscreenKeys);

   ui->power->setHidden(hideOnscreenKeys);

   //update current size
   QCoreApplication::postEvent(ui->centralWidget, resizeEvent);
}

bool MainWindow::eventFilter(QObject* object, QEvent* event){
   if(event->type() == QEvent::Resize){
      if(QString(object->metaObject()->className()) == "QPushButton"){
         QPushButton* button = (QPushButton*)object;

         button->setIconSize(QSize(button->size().width() / 1.7, button->size().height() / 1.7));
      }

      if(object->objectName() == "centralWidget"){
         float smallestRatio;
         bool hideOnscreenKeys = settings->value("hideOnscreenKeys", "").toBool();

         //update displayContainer first, make the display occupy the top 3/5 of the screen if there are Palm keys or 4/5 if theres not
         ui->displayContainer->setFixedHeight(ui->centralWidget->height() * (hideOnscreenKeys ? 0.80 : 0.60));

         smallestRatio = qMin(ui->displayContainer->size().width() * 0.98 / 3.0 , ui->displayContainer->size().height() * 0.98 / 4.0);
         //the 0.98 above allows the display to shrink, without it the displayContainer couldent shrink because of the fixed size of the display

         //set new size
         ui->display->setFixedSize(smallestRatio * 3.0, smallestRatio * 4.0);

         //scale framebuffer to new size and refresh
         if(emu.isInited()){
            ui->display->setPixmap(emu.getFramebuffer().scaled(QSize(ui->display->size().width(), ui->display->size().height()), Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->display->update();
         }
      }
   }

   return QMainWindow::eventFilter(object, event);
}

//display
void MainWindow::updateDisplay(){
   if(emu.newFrameReady()){
      //video, this is doing bilinear filitering in software, this is why the Qt port is broken on Android, move this to a new thread if possible
      ui->display->setPixmap(emu.getFramebuffer().scaled(ui->display->size().width(), ui->display->size().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

      //audio
      audioOut->write((const char*)emu.getAudioSamples(), AUDIO_SAMPLES_PER_FRAME * 2/*channels*/ * sizeof(int16_t));

      //power LED
      ui->powerButtonLed->setStyleSheet(emu.getPowerButtonLed() ? "background: lime" : "");

      //allow next frame to start
      emu.frameHandled();

      //update GUI
      ui->display->repaint();
      ui->powerButtonLed->repaint();
   }
}

//Palm buttons
void MainWindow::on_power_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_POWER, true);
}

void MainWindow::on_power_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_POWER, false);
}

void MainWindow::on_calendar_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_CALENDAR, true);
}

void MainWindow::on_calendar_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_CALENDAR, false);
}

void MainWindow::on_addressBook_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_ADDRESS, true);
}

void MainWindow::on_addressBook_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_ADDRESS, false);
}

void MainWindow::on_todo_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_TODO, true);
}

void MainWindow::on_todo_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_TODO, false);
}

void MainWindow::on_notes_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_NOTES, true);
}

void MainWindow::on_notes_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_NOTES, false);
}

void MainWindow::on_up_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_UP, true);
}

void MainWindow::on_up_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_UP, false);
}

void MainWindow::on_down_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_DOWN, true);
}

void MainWindow::on_down_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_DOWN, false);
}

void MainWindow::on_left_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_LEFT, true);
}

void MainWindow::on_left_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_LEFT, false);
}

void MainWindow::on_right_pressed(){
   emu.setKeyValue(EmuWrapper::BUTTON_RIGHT, true);
}

void MainWindow::on_right_released(){
   emu.setKeyValue(EmuWrapper::BUTTON_RIGHT, false);
}

//emu control
void MainWindow::on_ctrlBtn_clicked(){
   if(!emu.isInited()){
      uint32_t enabledFeatures = getEmuFeatureList();
      QString sysDir = settings->value("resourceDirectory", "").toString();
      uint32_t error = emu.init(sysDir + "/palmos41-en-m515.rom", QFile(sysDir + "/bootloader-en-m515.rom").exists() ? sysDir + "/bootloader-en-m515.rom" : "", sysDir + "/userdata-en-m515.ram", sysDir + "/sd-en-m515.img", enabledFeatures);

      if(error == EMU_ERROR_NONE){
         ui->up->setEnabled(true);
         ui->down->setEnabled(true);

         ui->calendar->setEnabled(true);
         ui->addressBook->setEnabled(true);
         ui->todo->setEnabled(true);
         ui->notes->setEnabled(true);

         ui->power->setEnabled(true);

         if(enabledFeatures & FEATURE_EXT_KEYS){
            ui->left->setEnabled(true);
            ui->right->setEnabled(true);
            ui->center->setEnabled(true);
         }

         ui->screenshot->setEnabled(true);
         ui->install->setEnabled(true);
         ui->stateManager->setEnabled(true);
         ui->debugger->setEnabled(true);
         ui->reset->setEnabled(true);

         ui->ctrlBtn->setIcon(QIcon(":/buttons/images/pause.svg"));
      }
      else{
         popupErrorDialog("Emu error:" + QString::number(error) + ", cant run!");
      }
   }
   else if(emu.isRunning()){
      emu.pause();
      ui->ctrlBtn->setIcon(QIcon(":/buttons/images/play.svg"));
   }
   else if(emu.isPaused()){
      emu.resume();
      ui->ctrlBtn->setIcon(QIcon(":/buttons/images/pause.svg"));
   }

   ui->ctrlBtn->repaint();
}

void MainWindow::on_install_clicked(){
   if(emu.isInited()){
      QString app = QFileDialog::getOpenFileName(this, "Select Application", QDir::root().path(), "Palm OS App (*.prc *.pdb *.pqa)");

      if(app != ""){
         uint32_t error = emu.installApplication(app);

         if(error != EMU_ERROR_NONE)
            popupErrorDialog("Could not install app, Error:" + QString::number(error));
      }
   }
}

void MainWindow::on_debugger_clicked(){
   if(emu.isInited()){
      emu.pause();
      ui->ctrlBtn->setIcon(QIcon(":/buttons/images/play.svg"));
      ui->ctrlBtn->repaint();
      emuDebugger->exec();
   }
}

void MainWindow::on_screenshot_clicked(){
   if(emu.isInited()){
      qlonglong screenshotNumber = settings->value("screenshotNum", 0).toLongLong();
      QString screenshotPath = settings->value("resourceDirectory", "").toString() + "/screenshots/screenshot" + QString::number(screenshotNumber, 10) + ".png";

      emu.getFramebuffer().save(screenshotPath, "PNG", 100);
      screenshotNumber++;
      settings->setValue("screenshotNum", screenshotNumber);
   }
}

void MainWindow::on_stateManager_clicked(){
   if(emu.isInited()){
      bool wasPaused = emu.isPaused();

      if(!wasPaused)
         emu.pause();

      stateManager->exec();

      if(!wasPaused)
         emu.resume();
   }
}

void MainWindow::on_reset_clicked(){
   emu.reset(false);
}

void MainWindow::on_settings_clicked(){
   bool wasInited = emu.isInited();
   bool wasPaused = emu.isPaused();

   if(wasInited && !wasPaused)
      emu.pause();

   settingsManager->exec();

   //redraw the main window
   redraw();

   //update keyboard settings too
   for(uint8_t index = 0; index < EmuWrapper::BUTTON_TOTAL_COUNT; index++)
      keyForButton[index] = settings->value("palmButton" + QString::number(index) + "Key", '\0').toInt();

   if(wasInited && !wasPaused)
      emu.resume();
}
