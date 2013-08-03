/*
 * Copyleft 2013 Zdeněk Janeček
 * ycdmdj@gmail.com
 *
 * Share it under GPL version 3
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <QtWidgets>
#include <QtDebug>
#include <QFile>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    moded = false;

    home.setPath(QDir::homePath().append(QDir::separator()).append(".qnotes"));
    if (!home.exists()) {
        home.setPath(QDir::homePath());
        home.mkdir(".qnotes");
        home.cd(".qnotes");
    }
    home.setFilter(QDir::Files | QDir::NoSymLinks);
    home.setSorting(QDir::Time);
    QFileInfoList list = home.entryInfoList();

    if (list.isEmpty()) {
        ui->listWidget->addItem(tr("Vítejte"));
        ui->listWidget->setCurrentRow(0);
        cur_note = tr("Vítejte");
        ui->plainTextEdit->setPlainText(tr("Tuto poznámku můžete klidně smazat."));
        save();
    } else {
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);

            ui->listWidget->addItem(fileInfo.fileName());
        }
        ui->listWidget->setCurrentRow(0);
        cur_note = ui->listWidget->item(0)->text();
        load();
    }

    connect(ui->actionUlozit, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionVypnout, SIGNAL(triggered()), this, SLOT(squit()));
    connect(ui->actionPridat, SIGNAL(triggered()), this, SLOT(add()));
    connect(ui->actionSmazat, SIGNAL(triggered()), this, SLOT(rm()));
    connect(ui->actionPrejmenovat, SIGNAL(triggered()), this, SLOT(ren()));
    connect(ui->plainTextEdit, SIGNAL(textChanged()), this, SLOT(mod()));
    connect(ui->actionO, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(change(QListWidgetItem*)));
    connect(ui->actionO_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::load()
{
    if (cur_note.isEmpty()) {
        ui->plainTextEdit->setPlainText("");
        ui->statusBar->showMessage("");
        moded = false;
        return -2;
    }

    QFile file(home.absolutePath().append(QDir::separator()).append(cur_note));

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nelze otevřít soubor"));
        return -1;
    }

    ui->plainTextEdit->setPlainText(file.readAll());
    file.close();

    ui->statusBar->showMessage("");
    moded = false;

    return 0;
}

bool MainWindow::save()
{
    if (cur_note.isEmpty())
        return -2;

    QFile file(home.absolutePath().append(QDir::separator()).append(cur_note));

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nelze zapisovat do souboru"));

        return -1;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << ui->plainTextEdit->toPlainText().toUtf8();
    file.flush();
    file.close();

    ui->statusBar->showMessage("");
    moded = false;

    return 0;
}

void MainWindow::add()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Jméno"),
                                         tr("Zadejte nadpis"), QLineEdit::Normal,
                                         QString(), &ok);
    if (!ok | name.isEmpty())
        return;

    QFile file(home.absolutePath().append(QDir::separator()).append(name));

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Chyba"), tr("Nelze zapisovat do souboru"));

        return;
    }
    file.close();

    ui->listWidget->addItem(name);
    cur_note = name;
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    load();
}

void MainWindow::rm()
{
    QMessageBox msgBox;
    msgBox.setText(tr("Opravdu chcete mazat?"));
    msgBox.setInformativeText(QString(tr("zvolena položka %1")).arg(cur_note));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setIcon(QMessageBox::Warning);

    if (msgBox.exec() == QMessageBox::Yes) {
        QFile file(home.absolutePath().append(QDir::separator()).append(cur_note));

        file.remove();

        QListWidgetItem *item;
        for (int var = 0; var < ui->listWidget->count(); ++var) {
            item = ui->listWidget->item(var);
            if (!item->text().compare(cur_note)) {
                delete ui->listWidget->takeItem(var);
                break;
            }
        }

        if (ui->listWidget->count() > 0) {
            cur_note = ui->listWidget->item(0)->text();
            ui->listWidget->setCurrentRow(0);
        } else {
            cur_note = "";
        }

        load();
    }
}

void MainWindow::ren()
{
    bool ok;
    QString name = QInputDialog::getText(this, QString(tr("Poznámka %1")).arg(cur_note),
                                         tr("Zadejte nové jméno"), QLineEdit::Normal,
                                         QString(), &ok);
    if (!ok | name.isEmpty())
        return;

    QFile file(home.absolutePath().append(QDir::separator()).append(cur_note));

    file.rename(home.absolutePath().append(QDir::separator()).append(name));

    QListWidgetItem *item;
    for (int var = 0; var < ui->listWidget->count(); ++var) {
        item = ui->listWidget->item(var);
        if (!item->text().compare(cur_note)) {
            item->setText(name);
            break;
        }
    }

    cur_note = name;
}

void MainWindow::cl()
{
    if (moded) {
        QMessageBox msgBox;
        msgBox.setText(tr("Dokument byl upraven."));
        msgBox.setInformativeText("Chcete uložit změny?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard);
        msgBox.setDefaultButton(QMessageBox::Save);
        msgBox.setIcon(QMessageBox::Information);
        int ret = msgBox.exec();

        switch (ret) {
          case QMessageBox::Save:
            // Save was clicked
            save();
            break;
          case QMessageBox::Discard:
            // Don't Save was clicked
            break;
          default:
              // should never be reached
              break;
        }
    }
}

void MainWindow::mod()
{
    if (moded) {
        // show some info
    } else {
        ui->statusBar->showMessage(tr("Upraveno"));
        moded = true;
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("O aplikaci"),
      tr("Aplikace <b>QNote</b> je jednoduchým poznámkovým blokem využívající Qt."
         "<br><br>Tato aplikace je šířena pod GPL verze 3."
         "<br><br>Zdeněk Janeček 2013"));
}

void MainWindow::squit()
{
    qApp->quit();
}

void MainWindow::change(QListWidgetItem *item)
{
    if (moded) {
        save();
        ui->statusBar->showMessage("");
        moded = false;
    }

    cur_note = item->text();

    load();
}
