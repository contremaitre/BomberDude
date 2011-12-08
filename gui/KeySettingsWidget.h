#ifndef KEYSETTINGSWIDGET_H
#define KEYSETTINGSWIDGET_H

#include "ui_KeySettingsWidget.h"
class Settings;

class KeySettingsWidget : public QWidget
{
    Q_OBJECT
public:
    KeySettingsWidget(Settings *settings, QWidget *parent = 0);
    ~KeySettingsWidget();
    Ui_KeySettingsWidget ui;
protected:
    void keyPressEvent(QKeyEvent *event);

private:
    void loadKeys();
    void listenKey();
    Settings *settings;
    bool keyListening;
    QString keyToString(int key);

private slots:
    void slotPl1Up();
};


#endif
