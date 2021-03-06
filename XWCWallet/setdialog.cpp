
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>
#include <QListView>
#include <QtMath>

#include "setdialog.h"
#include "ui_setdialog.h"
#include "wallet.h"
#include "AccountManagerWidget.h"
#include "HelpWidget.h"
#include "dialog/ErrorResultDialog.h"
#include "commondialog.h"

#define MIN_CONTRACT_FEE    1
#define MAX_CONTRACT_FEE    100


SetDialog::SetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetDialog)
{
	

    ui->setupUi(this);

//    XWCWallet::getInstance()->appendCurrentDialogVector(this);
    setParent(XWCWallet::getInstance()->mainFrame->containerWidget);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    ui->widget->setObjectName("widget");
    ui->widget->setStyleSheet(BACKGROUNDWIDGET_STYLE);
    ui->containerWidget->setObjectName("containerwidget");
    //ui->containerWidget->setStyleSheet(CONTAINERWIDGET_STYLE);
    ui->containerWidget->setStyleSheet("#containerwidget{background-color:rgb(255,255,255);border-top-right-radius:4px;border-bottom-right-radius:4px;border-bottom-left-radius:4px;}");

    ui->generalBtn->setCheckable(true);
    ui->safeBtn->setCheckable(true);
    ui->accountBtn->setCheckable(true);
    ui->depositBtn->setCheckable(true);
    ui->oldRpcBtn->setCheckable(true);

    ui->depositBtn->setChecked(XWCWallet::getInstance()->autoDeposit);
//    ui->depositBtn->setIconSize(QSize(26,12));
    ui->depositBtn->setIcon(ui->depositBtn->isChecked()?QIcon(":/ui/wallet_ui/on.png"):QIcon(":/ui/wallet_ui/off.png"));
    ui->depositBtn->setText(ui->depositBtn->isChecked()?tr("on"):tr("off"));
    ui->label_autotip->setWordWrap(true);
    ui->label_autotip->setVisible(ui->depositBtn->isChecked());

    ui->oldRpcBtn->setChecked(XWCWallet::getInstance()->oldRpcAdapter->isRunning());
    ui->oldRpcBtn->setIcon(ui->oldRpcBtn->isChecked()?QIcon(":/ui/wallet_ui/on.png"):QIcon(":/ui/wallet_ui/off.png"));
    ui->oldRpcBtn->setText(ui->oldRpcBtn->isChecked()?tr("on"):tr("off"));

    ui->generalBtn->setChecked(false);

    ui->checkbox_AutoBackup->setChecked(XWCWallet::getInstance()->autoBackupWallet);
//    ui->generalBtn->setIconSize(QSize(12,12));
//    ui->generalBtn->setIcon(QIcon(":/ui/wallet_ui/gray_circle.png"));

//    ui->safeBtn->setIconSize(QSize(12,12));
//    ui->safeBtn->setIcon(QIcon(":/ui/wallet_ui/gray_circle.png"));

//    ui->accountBtn->setIconSize(QSize(12,12));
//    ui->accountBtn->setIcon(QIcon(":/ui/wallet_ui/gray_circle.png"));


    setStyleSheet("QToolButton{background-color:transparent;border:none;color:rgb(83,107,215);font: 11px \"微软雅黑\";}"
                  "QToolButton::checked{color:rgb(90,115,227);}"

                  "QToolButton#toolButton_help,QToolButton#toolButton_set{font:12px \"微软雅黑\";border:none;background:transparent;color:white;border-top-left-radius:4px;border-top-right-radius:4px;}"
                  "QToolButton#toolButton_help::checked,QToolButton#toolButton_set::checked{color:rgb(80,80,80);background:#FFFFFF;}"

                  "QToolButton#depositBtn{color:rgb(84,61,137);}"


                  "QSpinBox::up-button {width:0;height:0;}"
                  "QSpinBox::down-button {width:0;height:0;}"
                  "QSpinBox::up-arrow {width:0;height:0;}"
                  "QSpinBox::down-arrow {width:0;height:0;}"
                  "QSpinBox{font: 11px \"微软雅黑\";background-color: white;border:1px solid rgb(84,61,137);border-radius:4px;color:rgb(56,36,88);height:24px;}"
                  "QSpinBox:focus{border:1px solid rgb(84,61,137);border-radius:4px;}"
                  "QSpinBox:disabled{background:transparent;color: rgb(83,90,109);border:1px solid gray;}"

                  );
    ui->saveBtn->setStyleSheet(OKBTN_STYLE);
    ui->confirmBtn->setStyleSheet(OKBTN_STYLE);
    ui->languageComboBox->setStyleSheet(COMBOBOX_BORDER_STYLE);
    ui->middlewareCombobox->setStyleSheet(COMBOBOX_BORDER_STYLE);
    ui->comboBox_fee->setStyleSheet(COMBOBOX_BORDER_STYLE);

    ui->generalBtn->setStyleSheet(PUSHBUTTON_CHECK_STYLE);
    ui->safeBtn->setStyleSheet(PUSHBUTTON_CHECK_STYLE);
    ui->accountBtn->setStyleSheet(PUSHBUTTON_CHECK_STYLE);

    ui->closeBtn->setIconSize(QSize(8,8));
    ui->closeBtn->setIcon(QIcon(":/ui/wallet_ui/white_close.png"));

    //ui->closeBtn->setStyleSheet(CLOSEBTN_STYLE);
    ui->closeBtn->setStyleSheet("QToolButton{background-color:rgb(90,115,227);}");


    ui->containerWidget->installEventFilter(this);

    connect( XWCWallet::getInstance(), SIGNAL(jsonDataUpdated(QString)), this, SLOT(jsonDataUpdated(QString)));

    on_generalBtn_clicked();

    ui->lockTimeSpinBox->setValue(QString::number(XWCWallet::getInstance()->lockMinutes).toInt());

    if(!XWCWallet::getInstance()->notProduce)
    {
        ui->nolockCheckBox->setChecked(true);
        ui->lockTimeSpinBox->setEnabled(false);
    }
    else
    {
        ui->nolockCheckBox->setChecked(false);
        ui->lockTimeSpinBox->setEnabled(true);
    }


    QString language = XWCWallet::getInstance()->language;
    if( language == "Simplified Chinese")
    {
        ui->languageComboBox->setCurrentIndex(0);
    }
    else if( language == "English")
    {
        ui->languageComboBox->setCurrentIndex(1);
    }
    else
    {
        ui->languageComboBox->setCurrentIndex(1);
    }

    QString fee = XWCWallet::getInstance()->feeType;
    foreach(AssetInfo asset,XWCWallet::getInstance()->assetInfoMap){
        ui->comboBox_fee->addItem(asset.symbol);
    }
    ui->feeCheckBox->setVisible(false);
    ui->comboBox_fee->setCurrentText(fee);


    ui->minimizeCheckBox->setChecked( XWCWallet::getInstance()->minimizeToTray);
    ui->closeCheckBox->setChecked( XWCWallet::getInstance()->closeToMinimize);
    ui->resyncCheckBox->setChecked( XWCWallet::getInstance()->resyncNextTime);

    ui->contractFeeLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->contractFeeLineEdit->setText( getBigNumberString(XWCWallet::getInstance()->contractFee,ASSET_PRECISION));

    QRegExp rx(QString("^([0]|[1-9][0-9]{0,2})(?:\\.\\d{0,%1})?$|(^\\t?$)").arg(ASSET_PRECISION));
    QRegExpValidator *pReg = new QRegExpValidator(rx, this);
    ui->contractFeeLineEdit->setValidator(pReg);

    ui->confirmBtn->setEnabled(false);

    ui->oldPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->newPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);
    ui->confirmPwdLineEdit->setContextMenuPolicy(Qt::NoContextMenu);

    ui->oldPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->newPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->confirmPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    ui->lockTimeSpinBox->setAttribute(Qt::WA_InputMethodEnabled, false);

    ui->dataPathLineEdit->setReadOnly(true);
    ui->configPathLineEdit->setReadOnly(true);
    ui->dataPathLineEdit->setText(XWCWallet::getInstance()->configFile->value("/settings/chainPath").toString());
    ui->configPathLineEdit->setText(XWCWallet::getInstance()->walletConfigPath);

    ui->middlewareCombobox->addItems(XWCWallet::getInstance()->getOfficialMiddleWareUrls());//->setText(XWCWallet::getInstance()->middlewarePath);
    if(!XWCWallet::getInstance()->getOfficialMiddleWareUrls().contains(XWCWallet::getInstance()->middlewarePath))
    {
        ui->middlewareCombobox->addItem(XWCWallet::getInstance()->middlewarePath);
    }
    ui->middlewareCombobox->setCurrentText(XWCWallet::getInstance()->middlewarePath);
    ui->middlewareCombobox->setEditable(true);

    ui->toolButton_help->setCheckable(true);
    ui->toolButton_set->setCheckable(true);
    ui->toolButton_set->setChecked(true);
    ui->toolButton_help->setChecked(false);

    //新建账户管理页面
//    AccountManagerWidget *accountManage = new AccountManagerWidget(ui->containerWidget);
//    ui->stackedWidget->addWidget(accountManage);

    //帮助页面
//    HelpWidget *helpWidget = new HelpWidget();
//    ui->stackedWidget_2->insertWidget(1,helpWidget);
    ui->label_helppic->setPixmap(QPixmap(":/ui/wallet_ui/help_pic.png").scaled(ui->label_helppic->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
    ui->label_helppic->setVisible(false);
    connect(ui->stackedWidget_2,&QStackedWidget::currentChanged,[this](){
        this->ui->label_helppic->setVisible(this->ui->stackedWidget_2->currentIndex() == 1);
    });
    XWCWallet::getInstance()->mainFrame->installBlurEffect(ui->stackedWidget_2);
}

SetDialog::~SetDialog()
{
    qDebug() << "setdialog delete";
//    XWCWallet::getInstance()->currentDialog = NULL;
    delete ui;
//    XWCWallet::getInstance()->removeCurrentDialogVector(this);
}

void SetDialog::pop()
{
//    QEventLoop loop;
//    show();
//    connect(this,SIGNAL(accepted()),&loop,SLOT(quit()));
//    loop.exec();  //进入事件 循环处理，阻塞

    move(0,0);
    exec();
}

void SetDialog::setHelpFirst(bool helpfirst)
{
    on_toolButton_help_clicked();
    if(helpfirst)
    {
        ui->page_help->CheckUpdateSlot();
    }
}


bool SetDialog::eventFilter(QObject *watched, QEvent *e)
{
    if( watched == ui->containerWidget)
    {
        if( e->type() == QEvent::Paint)
        {
            //QPainter painter(ui->containerWidget);
            //painter.setPen(QPen(QColor(40,45,66),Qt::SolidLine));
            //painter.setBrush(QBrush(QColor(40,45,66),Qt::SolidPattern));
            //painter.drawRoundedRect(0,20,600,400,10,10);
            //
            //painter.setPen(QPen(QColor(32,36,54),Qt::SolidLine));
            //painter.setBrush(QBrush(QColor(32,36,54),Qt::SolidPattern));
            //painter.drawRoundedRect(0,0,600,37,10,10);
            //painter.drawRect(0,20,600,17);
            //
            //return true;
        }
    }

    return QWidget::eventFilter(watched,e);
}

void SetDialog::on_closeBtn_clicked()
{
    close();
//    emit accepted();
}

void SetDialog::on_saveBtn_clicked()
{
    on_contractFeeLineEdit_editingFinished();

    mutexForConfigFile.lock();
    XWCWallet::getInstance()->lockMinutes = ui->lockTimeSpinBox->value();
    XWCWallet::getInstance()->configFile->setValue("/settings/lockMinutes", XWCWallet::getInstance()->lockMinutes);
    if( ui->nolockCheckBox->isChecked())
    {
        XWCWallet::getInstance()->notProduce = false;
    }
    else
    {
        XWCWallet::getInstance()->notProduce = true;
    }
    XWCWallet::getInstance()->configFile->setValue("/settings/notAutoLock", !XWCWallet::getInstance()->notProduce);

    if( ui->languageComboBox->currentIndex() == 0)      // config 中保存语言设置
    {
        XWCWallet::getInstance()->configFile->setValue("/settings/language", "Simplified Chinese");
        XWCWallet::getInstance()->language = "Simplified Chinese";
    }
    else if( ui->languageComboBox->currentIndex() == 1)
    {
        XWCWallet::getInstance()->configFile->setValue("/settings/language", "English");
        XWCWallet::getInstance()->language = "English";
    }


    XWCWallet::getInstance()->configFile->setValue("/settings/feeType", ui->comboBox_fee->currentText());
    XWCWallet::getInstance()->feeType = ui->comboBox_fee->currentText();


    XWCWallet::getInstance()->configFile->setValue("/settings/autoDeposit", ui->depositBtn->isChecked());
    XWCWallet::getInstance()->autoDeposit = ui->depositBtn->isChecked();


    XWCWallet::getInstance()->minimizeToTray = ui->minimizeCheckBox->isChecked();
    XWCWallet::getInstance()->configFile->setValue("/settings/minimizeToTray", XWCWallet::getInstance()->minimizeToTray);

    XWCWallet::getInstance()->closeToMinimize = ui->closeCheckBox->isChecked();
    XWCWallet::getInstance()->configFile->setValue("/settings/closeToMinimize", XWCWallet::getInstance()->closeToMinimize);

    XWCWallet::getInstance()->resyncNextTime = ui->resyncCheckBox->isChecked();
    XWCWallet::getInstance()->configFile->setValue("/settings/resyncNextTime", XWCWallet::getInstance()->resyncNextTime);

    XWCWallet::getInstance()->contractFee = ui->contractFeeLineEdit->text().toDouble() * qPow(10,ASSET_PRECISION);
    XWCWallet::getInstance()->configFile->setValue("/settings/contractFee", XWCWallet::getInstance()->contractFee);

    XWCWallet::getInstance()->middlewarePath = ui->middlewareCombobox->currentText();
    XWCWallet::getInstance()->configFile->setValue("/settings/middlewarePath", XWCWallet::getInstance()->middlewarePath);

    XWCWallet::getInstance()->autoBackupWallet = ui->checkbox_AutoBackup->isChecked();
    XWCWallet::getInstance()->configFile->setValue("/settings/autoBackupWallet", XWCWallet::getInstance()->autoBackupWallet);

    mutexForConfigFile.unlock();

    emit settingSaved();

    close();
//    emit accepted();

	
}


//void SetDialog::mousePressEvent(QMouseEvent *event)
//{
//    if(event->button() == Qt::LeftButton)
//     {
//          mouse_press = true;
//          //鼠标相对于窗体的位置（或者使用event->globalPos() - this->pos()）
//          move_point = event->pos();;
//     }
//}

//void SetDialog::mouseMoveEvent(QMouseEvent *event)
//{
//    //若鼠标左键被按下
//    if(mouse_press)
//    {
//        //鼠标相对于屏幕的位置
//        QPoint move_pos = event->globalPos();

//        //移动主窗体位置
//        this->move(move_pos - move_point);
//    }
//}

//void SetDialog::mouseReleaseEvent(QMouseEvent *)
//{
//    mouse_press = false;
//}

void SetDialog::on_nolockCheckBox_clicked()
{
    if(ui->nolockCheckBox->isChecked())
    {
        ui->lockTimeSpinBox->setEnabled(false);
    }
    else
    {
        ui->lockTimeSpinBox->setEnabled(true);
    }
}

void SetDialog::on_lockTimeSpinBox_valueChanged(const QString &arg1)
{
    if( arg1.startsWith('0') || arg1.isEmpty())
    {
        ui->lockTimeSpinBox->setValue(1);
    }
}

void SetDialog::on_generalBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    updateButtonIcon(0);
}

void SetDialog::on_depositBtn_clicked()
{
    if(ui->depositBtn->isChecked())
    {
        ui->depositBtn->setIcon(QIcon(":/ui/wallet_ui/on.png"));
        ui->depositBtn->setText(tr("on"));
    }
    else
    {
        ui->depositBtn->setIcon(QIcon(":/ui/wallet_ui/off.png"));
        ui->depositBtn->setText(tr("off"));
    }
    ui->label_autotip->setVisible(ui->depositBtn->isChecked());
}

void SetDialog::on_safeBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    updateButtonIcon(1);
}

void SetDialog::on_accountBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    updateButtonIcon(2);
}

void SetDialog::on_confirmBtn_clicked()
{
    ui->tipLabel3->clear();

    XWCWallet::getInstance()->postRPC( "id-unlock-SetDialog", toJsonFormat( "unlock", QJsonArray() << ui->oldPwdLineEdit->text() ));

}

void SetDialog::on_newPwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->newPwdLineEdit->setText( ui->newPwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->tipLabel->setText("");
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->newPwdLineEdit->text().length() < 8)
    {
//        ui->tipLabel->setText(QString::fromLocal8Bit("密码最少为8位"));
        ui->tipLabel->setText(tr("at least 8 letters"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else
    {
        ui->tipLabel->clear();
    }

    if( ui->confirmPwdLineEdit->text().isEmpty())
    {
        ui->tipLabel2->clear();
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->newPwdLineEdit->text() == ui->confirmPwdLineEdit->text())
    {
        ui->tipLabel2->clear();
        ui->confirmBtn->setEnabled(true);

    }
    else
    {
//        ui->tipLabel2->setText(QString::fromLocal8Bit("两次密码不一致"));
        ui->tipLabel2->setText(tr("not consistent"));
        ui->confirmBtn->setEnabled(false);

    }
}

void SetDialog::on_confirmPwdLineEdit_textChanged(const QString &arg1)
{
    if( arg1.indexOf(' ') > -1)
    {
        ui->confirmPwdLineEdit->setText( ui->confirmPwdLineEdit->text().remove(' '));
        return;
    }

    if( arg1.isEmpty())
    {
        ui->tipLabel2->setText("");
        ui->confirmBtn->setEnabled(false);

        return;
    }

    if( ui->confirmPwdLineEdit->text().length() < 8)
    {
//        ui->tipLabel2->setText(QString::fromLocal8Bit("两次密码不一致"));
        ui->tipLabel2->setText(tr("not consistent"));
        ui->tipLabel2->setText(tr("at least 8 letters"));
        ui->confirmBtn->setEnabled(false);

        return;
    }
    else
    {
        ui->tipLabel2->clear();
    }


    if( ui->newPwdLineEdit->text() == ui->confirmPwdLineEdit->text())
    {
        ui->confirmBtn->setEnabled(true);

    }
    else
    {
//        ui->tipLabel2->setText(QString::fromLocal8Bit("两次密码不一致"));
        ui->tipLabel2->setText(tr("not consistent"));
        ui->confirmBtn->setEnabled(false);

    }
}


void SetDialog::on_oldPwdLineEdit_textChanged(const QString &arg1)
{
    ui->tipLabel3->clear();

    if( arg1.indexOf(' ') > -1)
    {
        ui->oldPwdLineEdit->setText( ui->oldPwdLineEdit->text().remove( ' '));
    }
}

void SetDialog::jsonDataUpdated(QString id)
{
    if( id == "id-unlock-SetDialog")
    {
        QString result = XWCWallet::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            XWCWallet::getInstance()->postRPC( "id-set_password", toJsonFormat( "set_password", QJsonArray() << ui->newPwdLineEdit->text() ));
        }
        else
        {
            ui->tipLabel3->setText( tr("wrong password"));
        }

        return;
    }

    if( id == "id-set_password")
    {
        QString result = XWCWallet::getInstance()->jsonDataValue(id);

        if( result == "\"result\":null")
        {
            XWCWallet::getInstance()->postRPC( "id-unlock-pwdModified", toJsonFormat( "unlock", QJsonArray() << ui->newPwdLineEdit->text() ));

            close();
        }
        else
        {
            ErrorResultDialog errorResultDialog;
            errorResultDialog.setInfoText(tr("Fail to modify the password!"));
            errorResultDialog.setDetailText(result);
            errorResultDialog.pop();
        }

        return;
    }

}

void SetDialog::on_toolButton_set_clicked()
{
    ui->toolButton_set->setChecked(true);
    ui->toolButton_help->setChecked(false);
    ui->stackedWidget_2->setCurrentIndex(0);

}

void SetDialog::on_toolButton_help_clicked()
{
    ui->toolButton_set->setChecked(false);
    ui->toolButton_help->setChecked(true);
    ui->stackedWidget_2->setCurrentIndex(1);
}

void SetDialog::updateButtonIcon(int buttonNumber)
{
//    QIcon gray(":/ui/wallet_ui/gray_circle.png");
//    QIcon blue(":/ui/wallet_ui/blue_circle.png");
    ui->generalBtn->setChecked(0 == buttonNumber);
    ui->safeBtn->setChecked(1 == buttonNumber);
    ui->accountBtn->setChecked(2 == buttonNumber);

    QPixmap gray(":/ui/wallet_ui/gray_circle.png");
    gray = gray.scaled(4,4,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    QPixmap blue(":/ui/wallet_ui/blue_circle.png");
    blue = blue.scaled(12,12,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);

//    ui->generalBtn->setIcon(0 == buttonNumber ? blue : gray);
//    ui->safeBtn->setIcon(1 == buttonNumber ? blue : gray);
//    ui->accountBtn->setIcon(2 == buttonNumber ? blue : gray);
}

void SetDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

//    painter.setPen(Qt::NoPen);
//    painter.drawPixmap(rect(),QPixmap(":/ui/wallet_ui/back_dialog.png").scaled(rect().size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));

    QWidget::paintEvent(event);

}


//void SetDialog::setVisible(bool visiable)
//{
//    // 这样调用的是qwidget的setvisible 而不是qdialog的setvisible
//    // 即使调用hide 也不会结束exec(delete)
////    qDebug() << "setvisible " << visiable;
//    QWidget::setVisible(visiable);
//}

void SetDialog::on_contractFeeLineEdit_editingFinished()
{
    if(ui->contractFeeLineEdit->text().toDouble() < getBigNumberString(MIN_CONTRACT_FEE,ASSET_PRECISION).toDouble())
    {
        ui->contractFeeLineEdit->setText(getBigNumberString(MIN_CONTRACT_FEE,ASSET_PRECISION));
    }
    else if(ui->contractFeeLineEdit->text().toDouble() > getBigNumberString(MAX_CONTRACT_FEE,ASSET_PRECISION).toDouble())
    {
        ui->contractFeeLineEdit->setText(getBigNumberString(MAX_CONTRACT_FEE,ASSET_PRECISION));
    }

}

void SetDialog::on_pathBtn_clicked()
{
#ifdef WIN32
            QProcess::startDetached("explorer \"" + ui->dataPathLineEdit->text() + "\"");
#else
            QProcess::startDetached("open \"" + ui->dataPathLineEdit->text() + "\"");
#endif
}

void SetDialog::on_pathBtn2_clicked()
{
#ifdef WIN32
            QProcess::startDetached("explorer \"" + ui->configPathLineEdit->text() + "\"");
#else
            QProcess::startDetached("open \"" + ui->configPathLineEdit->text() + "\"");
#endif
}

void SetDialog::on_oldRpcBtn_clicked()
{
    if(ui->oldRpcBtn->isChecked())
    {
        ui->oldRpcBtn->setIcon(QIcon(":/ui/wallet_ui/on.png"));
        ui->oldRpcBtn->setText(tr("on"));

        XWCWallet::getInstance()->oldRpcAdapter->startServer();
    }
    else
    {
        ui->oldRpcBtn->setIcon(QIcon(":/ui/wallet_ui/off.png"));
        ui->oldRpcBtn->setText(tr("off"));

        XWCWallet::getInstance()->oldRpcAdapter->close();
    }
}
