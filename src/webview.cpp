/*
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 * Copyright 2008 Jason A. Donenfeld <Jason@zx2c4.com>
 * Copyright 2008 Ariya Hidayat <ariya.hidayat@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "cookiejar.h"
#include "downloadmanager.h"
#include "networkaccessmanager.h"
#include "tabwidget.h"
#include "webview.h"
#include "bookmarks.h"

#include <qbuffer.h>
#include <qclipboard.h>
#include <qdesktopservices.h>
#include <qevent.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qsettings.h>
#include <qmenubar.h>

#include <qwebframe.h>

#include <QDesktopWidget>

#include <qdebug.h>

WebPage::WebPage(QObject *parent)
    : QWebPage(parent)
    , m_keyboardModifiers(Qt::NoModifier)
    , m_pressedButtons(Qt::NoButton)
    , m_openInNewTab(false)
    , mobileUserAgent(false)
{
    setNetworkAccessManager(BrowserApplication::networkAccessManager());
    connect(this, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(handleUnsupportedContent(QNetworkReply *)));
}

void WebPage::setMobileUserAgent(bool en)
{
    mobileUserAgent = en;
}

QString WebPage::userAgentForUrl(const QUrl& url) const
{
    if (mobileUserAgent)
    {
        return "Mozilla/5.0 (iPhone; U; CPU like Mac OS X; en) "
               "AppleWebKit/420+ (KHTML, like Gecko) Version/3.0 Mobile/1A538b Safari/419.3";
    }
    return QWebPage::userAgentForUrl(url);
}

BrowserMainWindow *WebPage::mainWindow()
{
    QObject *w = this->parent();
    while (w) {
        if (BrowserMainWindow *mw = qobject_cast<BrowserMainWindow*>(w))
            return mw;
        w = w->parent();
    }
    return BrowserApplication::instance()->mainWindow();
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type)
{
    // ctrl open in new tab
    // ctrl-shift open in new tab and select
    // ctrl-alt open in new window
    if (type == QWebPage::NavigationTypeLinkClicked
        && (m_keyboardModifiers & Qt::ControlModifier
            || m_pressedButtons == Qt::MidButton)) {
        bool newWindow = (m_keyboardModifiers & Qt::AltModifier);
        WebView *webView;
        if (newWindow) {
            BrowserApplication::instance()->newMainWindow();
            BrowserMainWindow *newMainWindow = BrowserApplication::instance()->mainWindow();
            webView = newMainWindow->currentTab();
            newMainWindow->raise();
            newMainWindow->activateWindow();
            webView->setFocus();
        } else {
            bool selectNewTab = (m_keyboardModifiers & Qt::ShiftModifier);
            webView = mainWindow()->tabWidget()->makeNewTab(selectNewTab);
        }
        webView->loadUrl(request);
        m_keyboardModifiers = Qt::NoModifier;
        m_pressedButtons = Qt::NoButton;
        return false;
    }
    if (frame == mainFrame()) {
        m_loadingUrl = request.url();
        emit loadingUrl(m_loadingUrl);
    }

    QString scheme = request.url().scheme();
    if (scheme == QLatin1String("mailto")
        || scheme == QLatin1String("ftp")) {
        QDesktopServices::openUrl(request.url());
        return false;
    }

    return QWebPage::acceptNavigationRequest(frame, request, type);
}

QWebPage *WebPage::createWindow(QWebPage::WebWindowType type)
{
    Q_UNUSED(type);
    if (m_keyboardModifiers & Qt::ControlModifier || m_pressedButtons == Qt::MidButton)
        m_openInNewTab = true;
    if (m_openInNewTab) {
        m_openInNewTab = false;
        return mainWindow()->tabWidget()->makeNewTab()->page();
    }
    BrowserApplication::instance()->newMainWindow();
    BrowserMainWindow *mainWindow = BrowserApplication::instance()->mainWindow();
    return mainWindow->currentTab()->page();
}

#if !defined(QT_NO_UITOOLS)
QObject *WebPage::createPlugin(const QString &classId, const QUrl &url, const QStringList &paramNames, const QStringList &paramValues)
{
    Q_UNUSED(url);
    Q_UNUSED(paramNames);
    Q_UNUSED(paramValues);
    QUiLoader loader;
    return loader.createWidget(classId, view());
}
#endif // !defined(QT_NO_UITOOLS)

void WebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::ProtocolUnknownError) {
        QSettings settings;
        settings.beginGroup(QLatin1String("WebView"));
        QStringList externalSchemes;
        externalSchemes = settings.value(QLatin1String("externalSchemes")).toStringList();
        if (externalSchemes.contains(reply->url().scheme()))
            QDesktopServices::openUrl(reply->url());
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        if (reply->header(QNetworkRequest::ContentTypeHeader).isValid())
            BrowserApplication::downloadManager()->handleUnsupportedContent(reply);
        return;
    }

    QFile file(QLatin1String(":/notfound.html"));
    bool isOpened = file.open(QIODevice::ReadOnly);
    Q_ASSERT(isOpened);
    QString title = tr("Error loading page: %1").arg(reply->url().toString());
    QString html = QString(QLatin1String(file.readAll()))
                        .arg(title)
                        .arg(reply->errorString())
                        .arg(reply->url().toString());

    QBuffer imageBuffer;
    imageBuffer.open(QBuffer::ReadWrite);
    QIcon icon = view()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, view());
    QPixmap pixmap = icon.pixmap(QSize(32, 32));
    if (pixmap.save(&imageBuffer, "PNG")) {
        html.replace(QLatin1String("IMAGE_BINARY_DATA_HERE"),
                     QString(QLatin1String(imageBuffer.buffer().toBase64())));
    }

    QList<QWebFrame*> frames;
    frames.append(mainFrame());
    while (!frames.isEmpty()) {
        QWebFrame *frame = frames.takeFirst();
        if (frame->url() == reply->url()) {
            frame->setHtml(html, reply->url());
            return;
        }
        QList<QWebFrame *> children = frame->childFrames();
        foreach (QWebFrame *frame, children)
            frames.append(frame);
    }
    if (m_loadingUrl == reply->url()) {
        mainFrame()->setHtml(html, reply->url());
    }
}


WebView::WebView(QWidget *parent)
    : QWebView(parent)
    , m_progress(0)
    , m_currentZoom(133) //100
    , m_page(new WebPage(this))
    , fingerScrolling(true)
{
    setPage(m_page);
    connect(page(), SIGNAL(statusBarMessage(const QString&)),
            SLOT(setStatusBarText(const QString&)));
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(setProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(loadFinished()));
    connect(page(), SIGNAL(loadingUrl(const QUrl&)),
            this, SIGNAL(urlChanged(const QUrl &)));
    connect(page(), SIGNAL(downloadRequested(const QNetworkRequest &)),
            this, SLOT(downloadRequested(const QNetworkRequest &)));
    page()->setForwardUnsupportedContent(true);
    setAcceptDrops(true);

    target = 0;
    filterPress = false;
    pressed = false;
    moveThreshold = QApplication::desktop()->screenGeometry().width()/40;

    QtopiaApplication::setInputMethodHint(this,QtopiaApplication::Text);

    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(255, 255, 255, 255));
    p.setColor(QPalette::Background, QColor(255, 255, 255, 255));
    setPalette(p);
    setAttribute(Qt::WA_OpaquePaintEvent, false); //

    //setStyleSheet("* { background-color:rgb(255,255,255); color:rgb(0,0,0)}");

    /*
    QPalette pal = palette();
    pal.setBrush(QPalette::Background, Qt::white);
    pal.setBrush(QPalette::Base, QColor(255,255,255,255));
    pal.setBrush(QPalette::Foreground, Qt::black);
    pal.setBrush(QPalette::Text, Qt::black);
    pal.setBrush(QPalette::Button, QColor(220,220,220));
    setPalette(pal);
    setAttribute(Qt::WA_OpaquePaintEvent, false);
    */

    // the zoom values (in percent) are chosen to be like in Mozilla Firefox 3
    m_zoomLevels << 30 << 50 << 67 << 80 << 90;
    m_zoomLevels << 100;
    m_zoomLevels << 110 << 120 << 133 << 150 << 170 << 200 << 240 << 300;

    setZoomFactor(qreal(m_currentZoom) / 100.0);
}
/*
void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);

    QWebHitTestResult r = page()->mainFrame()->hitTestContent(event->pos());

    if (!r.linkUrl().isEmpty()) {
        menu->addAction(tr("Open in New &Window"), this, SLOT(openLinkInNewWindow()));
        menu->addAction(tr("Open in New &Tab"), this, SLOT(openLinkInNewTab()));
        menu->addSeparator();
        menu->addAction(tr("Save Lin&k"), this, SLOT(downloadLinkToDisk()));
        menu->addAction(tr("&Bookmark This Link"), this, SLOT(bookmarkLink()))->setData(r.linkUrl().toString());
        menu->addSeparator();
        menu->addAction(tr("&Copy Link Location"), this, SLOT(copyLinkToClipboard()));
        if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
            menu->addAction(pageAction(QWebPage::InspectElement));
    }

    if (!r.imageUrl().isEmpty()) {
        if (!menu->isEmpty())
            menu->addSeparator();
        menu->addAction(tr("Open Image in New &Window"), this, SLOT(openImageInNewWindow()));
        menu->addAction(tr("Open Image in New &Tab"), this, SLOT(openImageInNewTab()));
        menu->addSeparator();
        menu->addAction(tr("&Save Image"), this, SLOT(downloadImageToDisk()));
        menu->addAction(tr("&Copy Image"), this, SLOT(copyImageToClipboard()));
        menu->addAction(tr("C&opy Image Location"), this, SLOT(copyImageLocationToClipboard()))->setData(r.imageUrl().toString());
    }

#ifdef WEBKIT_TRUNK // i.e. Qt 4.5, but not in Qt 4.5 yet
    if (menu->isEmpty())
        menu = page()->createStandardContextMenu();
#endif

    if (!menu->isEmpty()) {
        if (m_page->mainWindow()->menuBar()->isHidden()) {
            menu->addSeparator();
            menu->addAction(m_page->mainWindow()->showMenuBarAction());
        }

        menu->exec(mapToGlobal(event->pos()));
        delete menu;
        return;
    }
    delete menu;

    QWebView::contextMenuEvent(event);
}
*/
void WebView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        int numDegrees = event->delta() / 8;
        int numSteps = numDegrees / 15;
        setZoomFactor(m_currentZoom + numSteps * 0.1);
        event->accept();
        return;
    }
    QWebView::wheelEvent(event);
}

void WebView::resizeEvent(QResizeEvent *event)
{
    int offset = event->size().height() - event->oldSize().height();
    int currentValue = page()->mainFrame()->scrollBarValue(Qt::Vertical);
    setUpdatesEnabled(false);
    page()->mainFrame()->setScrollBarValue(Qt::Vertical, currentValue - offset);
    setUpdatesEnabled(true);
    QWebView::resizeEvent(event);
}

void WebView::openLinkInNewTab()
{
    m_page->m_openInNewTab = true;
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::openLinkInNewWindow()
{
    m_page->m_openInNewTab = false;
    pageAction(QWebPage::OpenLinkInNewWindow)->trigger();
}

void WebView::downloadLinkToDisk()
{
    pageAction(QWebPage::DownloadLinkToDisk)->trigger();
}

void WebView::copyLinkToClipboard()
{
    pageAction(QWebPage::CopyLinkToClipboard)->trigger();
}

void WebView::openImageInNewTab()
{
    m_page->m_openInNewTab = true;
    pageAction(QWebPage::OpenImageInNewWindow)->trigger();
}

void WebView::openImageInNewWindow()
{
    pageAction(QWebPage::OpenImageInNewWindow)->trigger();
}

void WebView::downloadImageToDisk()
{
    pageAction(QWebPage::DownloadImageToDisk)->trigger();
}

void WebView::copyImageToClipboard()
{
    pageAction(QWebPage::CopyImageToClipboard)->trigger();
}

void WebView::copyImageLocationToClipboard()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        BrowserApplication::clipboard()->setText(action->data().toString());
    }
}

void WebView::bookmarkLink()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        AddBookmarkDialog dialog(action->data().toString(), QString());
        dialog.exec();
    }
}

void WebView::setProgress(int progress)
{
    m_progress = progress;
}

void WebView::zoomIn()
{
    int i = m_zoomLevels.indexOf(m_currentZoom);
    Q_ASSERT(i >= 0);

    if (i < m_zoomLevels.count() - 1)
        m_currentZoom = m_zoomLevels[i + 1];

    setZoomFactor(qreal(m_currentZoom) / 100.0);
}

void WebView::zoomOut()
{
    int i = m_zoomLevels.indexOf(m_currentZoom);
    Q_ASSERT(i >= 0);

    if (i > 0)
        m_currentZoom = m_zoomLevels[i - 1];

    setZoomFactor(qreal(m_currentZoom) / 100.0);
}

void WebView::resetZoom()
{
    m_currentZoom = 100;
    setZoomFactor(1.0);
}

void WebView::loadFinished()
{
    if (100 != m_progress) {
        qWarning() << "Recieved finished signal while progress is still:" << progress()
                   << "Url:" << url();
    }
    m_progress = 0;
}

void WebView::loadUrl(const QUrl &url, const QString &title)
{
    if (url.scheme() == QLatin1String("javascript")) {
        QString scriptSource = url.toString().mid(11);
        QVariant result = page()->mainFrame()->evaluateJavaScript(scriptSource);
        if (result.canConvert(QVariant::String)) {
            QString newHtml = result.toString();
            setHtml(newHtml);
        }
        return;
    }
    m_initialUrl = url;
    if (!title.isEmpty())
        emit titleChanged(tr("Loading..."));
    else
        emit titleChanged(title);
    load(url);
}

void WebView::loadUrl(const QNetworkRequest &request, QNetworkAccessManager::Operation operation, const QByteArray &body)
{
    m_initialUrl = request.url();
    emit titleChanged(tr("Loading..."));
    QWebView::load(request, operation, body);
}

QString WebView::lastStatusBarText() const
{
    return m_statusBarText;
}

QUrl WebView::url() const
{
    QUrl url = QWebView::url();
    if (!url.isEmpty())
        return url;

    return m_initialUrl;
}

/*
void WebView::mousePressEvent(QMouseEvent *event)
{
    m_page->m_pressedButtons = event->buttons();
    m_page->m_keyboardModifiers = event->modifiers();
    QWebView::mousePressEvent(event);
}

void WebView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void WebView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!event->mimeData()->urls().isEmpty()) {
        event->acceptProposedAction();
    } else {
        QUrl url(event->mimeData()->text());
        if (url.isValid())
            event->acceptProposedAction();
    }
    if (!event->isAccepted())
        QWebView::dragMoveEvent(event);
}

void WebView::dropEvent(QDropEvent *event)
{
    QWebView::dropEvent(event);
    if (!event->isAccepted()
        && event->possibleActions() & Qt::CopyAction) {

        QUrl url;
        if (!event->mimeData()->urls().isEmpty())
            url = event->mimeData()->urls().first();
        if (!url.isValid())
            url = event->mimeData()->text();
        if (url.isValid()) {
            loadUrl(url);
            event->acceptProposedAction();
        }
    }
}

void WebView::mouseReleaseEvent(QMouseEvent *event)
{
    QWebView::mouseReleaseEvent(event);
    if (!event->isAccepted() && (m_page->m_pressedButtons & Qt::MidButton)) {
        QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
        if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty()) {
            loadUrl(url);
        }
    }
}
*/

void WebView::setStatusBarText(const QString &string)
{
    m_statusBarText = string;
}

void WebView::downloadRequested(const QNetworkRequest &request)
{
    BrowserApplication::downloadManager()->download(request);
}

//===============================================================================

void WebView::setFingerScrolling(bool en)
{
    fingerScrolling = en;
    if (!en)
    {
        ptimer.stop();
    }
}

void WebView::timerEvent(QTimerEvent *ev)
{
    if (!fingerScrolling) return;

    if (ev->timerId() == ptimer.timerId()) {
        ptimer.stop();
        QApplication::postEvent(target, new QMouseEvent(QEvent::MouseButtonPress, target->mapFromGlobal(mousePos), mousePos, Qt::LeftButton, buttons, QApplication::keyboardModifiers()));
        if (!pressed)
            QApplication::postEvent(target, new QMouseEvent(QEvent::MouseButtonRelease, target->mapFromGlobal(mousePos), mousePos, Qt::LeftButton, buttons, QApplication::keyboardModifiers()));
    } else if (ev->timerId() == vcursorfade.timerId()) {
        QRect r(vcursorpos,vcursorpm.size());
        r.translate(-vcursorhotspot);
        vcursorfade.stop();
        update(r);
    }
}

void WebView::mousePressEvent(QMouseEvent* ev)
{
    if (!fingerScrolling)
    {
        QWebView::mousePressEvent(ev);
        return;
    }

    if (!ev->spontaneous()) {
        QWebView::mousePressEvent(ev);
        //updateSoftMenuBar();
        return;
    }
    if (ev->button() == Qt::LeftButton) {
        target = this;
        mousePos = ev->globalPos();
        buttons = ev->buttons();
        filterPress = false;
        ptimer.start(250, this);
        pressed = true;
        return;
    } else {
        target = 0;
        filterPress = false;
    }

    QWebView::mousePressEvent(ev);
    //updateSoftMenuBar();
}

void WebView::mouseMoveEvent(QMouseEvent* ev)
{
    if (!fingerScrolling)
    {
        QWebView::mouseMoveEvent(ev);
        return;
    }

    if (!ev->spontaneous()) {
        //QWebView::mouseMoveEvent(ev);
        return;
    }
    if (pressed) {
        QPoint diff = mousePos - ev->globalPos();
        if (!filterPress
            && (qAbs(diff.y()) > moveThreshold
            || qAbs(diff.x()) > moveThreshold)) {
            filterPress = true;
            if (!ptimer.isActive()) {
                // TODO: unpress buttons, etc
                // QThumbStyle uses bogus move to get e.g. QPushButtons unpressed.
                // but can cause selection to happen in webkit
            }
            ptimer.stop();
            diff = QPoint(0,0); // avoid jump
        }
        if (filterPress) {
            int maxh = page()->mainFrame()->scrollBarMaximum(Qt::Horizontal);
            int maxv = page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
            if (diff.y() && maxv > 0) {
                int moveY = diff.y();
                page()->mainFrame()->setScrollBarValue(Qt::Vertical, page()->mainFrame()->scrollBarValue(Qt::Vertical) + moveY);
            }

            if (diff.x() && maxh > 0) {
                int moveX = diff.x();
                page()->mainFrame()->setScrollBarValue(Qt::Horizontal, page()->mainFrame()->scrollBarValue(Qt::Horizontal) + moveX);
            }

            mousePos = ev->globalPos();
            return;
        }
        if (ptimer.isActive())
            return;
    }

    QWebView::mouseMoveEvent(ev);
}

void WebView::mouseReleaseEvent(QMouseEvent* ev)
{
    if (!fingerScrolling)
    {
        QWebView::mouseReleaseEvent(ev);
        return;
    }

    if (!ev->spontaneous()) {
        QWebView::mouseReleaseEvent(ev);
        //updateSoftMenuBar();
        return;
    }
    if (ev->button() == Qt::LeftButton) {
        pressed = false;
        if (target) {
            //scrollArea = 0;
            if (filterPress) {
                // Don't send any release
                target = 0;
                ptimer.stop();
                ev->accept();
                filterPress = false;
                return;
            }
            if (ptimer.isActive()) {
                ptimer.start(0, this);
                return;
            }
        }
    }

    QWebView::mouseReleaseEvent(ev);
    //updateSoftMenuBar();
}
