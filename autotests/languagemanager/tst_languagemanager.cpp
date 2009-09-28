/**
 * Copyright (c) 2008, Benjamin C. Meyer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <QtTest/QtTest>
#include <languagemanager.h>

class tst_LanguageManager : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void languagemanager_data();
    void languagemanager();

    void chooseNewLanguage_data();
    void chooseNewLanguage();
    void setCurrentLanguage_data();
    void setCurrentLanguage();
};

// Subclass that exposes the protected functions.
class SubLanguageManager : public LanguageManager
{
public:

};

class TestWidget : public QWidget
{
public:
    void changeEvent(QEvent *event) {
        if (event->type() == QEvent::LanguageChange)
            retranslate = true;
        QWidget::changeEvent(event);
    }
    bool retranslate;
};


// This will be called before the first test function is executed.
// It is only called once.
void tst_LanguageManager::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_LanguageManager::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_LanguageManager::init()
{
}

// This will be called after every test function.
void tst_LanguageManager::cleanup()
{
}

void tst_LanguageManager::languagemanager_data()
{
}

void tst_LanguageManager::languagemanager()
{
    SubLanguageManager manager;
    manager.languages();
    // spawns an event loop...
    // manager.chooseNewLanguage();
    QCOMPARE(manager.currentLanguage(), QString());
    manager.setCurrentLanguage(QString());
}

void tst_LanguageManager::chooseNewLanguage_data()
{
    QTest::addColumn<int>("foo");
    QTest::newRow("null") << 0;
}

// public void chooseNewLanguage()
void tst_LanguageManager::chooseNewLanguage()
{
    // how do you test this?
#if 0
    QFETCH(int, foo);

    SubLanguageManager manager;

    manager.chooseNewLanguage();
#endif
    QSKIP("Test is not implemented.", SkipAll);
}

void tst_LanguageManager::setCurrentLanguage_data()
{
    QTest::addColumn<QString>("language");
    QTest::addColumn<QString>("result");
    QTest::newRow("null") << QString("foo") << QString();

    SubLanguageManager manager;
    QString validLanguage = manager.languages().value(0);
    if (validLanguage.isEmpty())
        qWarning() << "no languages to test with";
    QTest::newRow(validLanguage.toLatin1()) << validLanguage << validLanguage;
}

// public void setCurrentLanguage(QString const &language)
void tst_LanguageManager::setCurrentLanguage()
{
    QFETCH(QString, language);
    QFETCH(QString, result);

    SubLanguageManager manager;
    QString initialLanguage = manager.currentLanguage();

    TestWidget widget;
    widget.retranslate = false;
    manager.setCurrentLanguage(language);
    QCOMPARE(manager.currentLanguage(), result);
    qApp->processEvents();
    QCOMPARE(widget.retranslate, !result.isEmpty());
}

QTEST_MAIN(tst_LanguageManager)
#include "tst_languagemanager.moc"

