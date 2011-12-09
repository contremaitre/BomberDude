#ifndef KEYSETTINGSWIDGET_H
#define KEYSETTINGSWIDGET_H

#include "ui_KeySettingsWidget.h"
#include "Settings.h"

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
    void focusOutEvent(QFocusEvent *event);
private:
    void loadKeys();
    void listenKey(const QString &);
    void stopListening();
    void changeKey(int key);
    Settings *settings;
    bool keyListening;
    key_set_t listenedKey;
    int listenedPlayer; /* 1 ou 2 */
    key_set_t player1_keys;
    key_set_t player2_keys;
    QString keyToString(int key);

private slots:
    void slotPl1Up();
    void slotPl1Left();
    void slotPl1Right();
    void slotPl1Down();
    void slotPl1Drop();
    void slotPl1Opt();
    void slotPl2Up();
    void slotPl2Left();
    void slotPl2Right();
    void slotPl2Down();
    void slotPl2Drop();
    void slotPl2Opt();
};


#endif
