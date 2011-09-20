/* This file is part of Clementine.
   Copyright 2010, David Sansome <me@davidsansome.com>

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GROOVESHARKSERVICE_H
#define GROOVESHARKSERVICE_H

#include "internetmodel.h"
#include "internetservice.h"

class GrooveSharkUrlHandler;
class NetworkAccessManager;
class Playlist;
class QMenu;
class QSortFilterProxyModel;
class QNetworkRequest;

class GrooveSharkService : public InternetService {
  Q_OBJECT
 public:
  GrooveSharkService(InternetModel *parent);
  ~GrooveSharkService();

  enum Type {
    Type_SearchResults = InternetModel::TypeCount
  };

  // Values are persisted - don't change.
  enum LoginState {
    LoginState_LoggedIn = 1,
    LoginState_AuthFailed = 2,
    LoginState_NoPremium = 3,
    LoginState_OtherError = 4
  };

  // Internet Service methods
  QStandardItem* CreateRootItem();
  void LazyPopulate(QStandardItem *parent);

  void ItemDoubleClicked(QStandardItem* item);
  void ShowContextMenu(const QModelIndex& index, const QPoint& global_pos);

  void Search(const QString& text, Playlist* playlist, bool now = false);
  // User should be logged in to be able to generate streaming urls
  QUrl GetStreamingUrlFromSongId(const QString& song_id);
  void Login(const QString& username, const QString& password);
  void Logout();
  bool IsLoggedIn() { return !session_id_.isEmpty(); }
  // Persisted in the settings and updated on each Login().
  LoginState login_state() const { return login_state_; }
  const QString& session_id() { return session_id_; }


  static const char* kServiceName;
  static const char* kSettingsGroup;
  static const char* kUrl;

  static const int kSongSearchLimit;
  static const int kSearchDelayMsec;

  static const char* kApiKey;
  static const char* kApiSecret;

 signals:
  void LoginFinished(bool success);

 protected:
  QModelIndex GetCurrentIndex();

 private slots:
  void UpdateTotalSongCount(int count);

  void SessionCreated();
  void DoSearch();
  void SearchSongsFinished();
  void Authenticated();

 private:
  void EnsureMenuCreated();
  void EnsureConnected();

  void OpenSearchTab();
  void AuthenticateSession();
  void InitCountry();

  // Create a request for the given method, with the given params.
  // If need_authentication is true, add session_id to params.
  // Returns the reply object created
  QNetworkReply* CreateRequest(const QString& method_name, const QList<QPair<QString, QVariant> > params,
                     bool need_authentication = false,
                     bool use_https = false);
  // Convenient function for extracting result from reply
  QVariantMap ExtractResult(QNetworkReply* reply);
  void ResetSessionId();


  GrooveSharkUrlHandler* url_handler_;
  QString pending_search_;
  Playlist* pending_search_playlist_;

  QStandardItem* root_;
  QStandardItem* search_;

  NetworkAccessManager* network_;

  QMenu* context_menu_;
  QModelIndex context_item_;

  QTimer* search_delay_;
  QNetworkReply* last_search_reply_;

  QString username_;
  QString password_; // In fact, password's md5 hash
  QString user_id_;
  QString session_id_;
  QMap<QString, QVariant> country_;
  QByteArray api_key_;

  LoginState login_state_;
};


#endif // GROOVESHARKSERVICE_H