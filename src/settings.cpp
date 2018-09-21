/*
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 *
 * Partialy financiated as part of the protocol between UTAustin I Portugal - UTA-P.
 * [2017] - [2018] University of Minho, Filipe Costa Oliveira 
 * All Rights Reserved.
 */

#include "settings.h"
#include "ui_settings.h"

#include <iostream>

Settings::Settings(QWidget *parent) :  QDialog(parent), ui(new Ui::Settings) {
  ui->setupUi(this);
  _tree_delegate = new TreeItemFileDelegate(this);
}

Settings::~Settings(){
  delete ui;
}

bool Settings::set_dr_probe_bin_path( std::string path ){
  _dr_probe_bin_path  = QString::fromStdString(path);
  return true;
}

bool Settings::set_q_settings_fileName( std::string filename ){
  _q_settings_fileName  = QString::fromStdString(filename);
  QString label ("Settings file: ");
  QString appDir ("Application dir: ");
  QString workDir ( "Current path: " );
  ui->qlabel_settings->setText(
               appDir + QCoreApplication::applicationDirPath()
              + "\n" + workDir + QDir::currentPath()
              + "\n" + label + _q_settings_fileName
              );
  return true;
}

bool Settings::set_dr_probe_celslc_bin( std::string bin ){
  _dr_probe_celslc_bin  = QString::fromStdString(bin);
  return true;
}

bool Settings::set_dr_probe_msa_bin( std::string bin ){
  _dr_probe_msa_bin  = QString::fromStdString(bin);
  return true;
}

bool Settings::set_dr_probe_wavimg_bin( std::string bin ){
  _dr_probe_wavimg_bin  = QString::fromStdString(bin);
  return true;
}


bool Settings::set_im2model_api_url( std::string url ){
  im2model_api_url  = QString::fromStdString(url);
  return true;
}


QString Settings::get_im2model_api_url(){
  return im2model_api_url;
}

QString Settings::get_dr_probe_bin_path(){
  return _dr_probe_bin_path;
}

QString Settings::get_dr_probe_celslc_bin( ){
  return _dr_probe_celslc_bin;
}

QString Settings::get_dr_probe_msa_bin( ){
  return _dr_probe_msa_bin;
}

QString Settings::get_dr_probe_wavimg_bin( ){
  return _dr_probe_wavimg_bin;
}

void Settings::on_buttonBox_accepted()
{
  _save_preferences = true;
}

bool Settings::_is_save_preferences(){
  return _save_preferences;
}

void Settings::produce_settings_panel(){

  QVector<QVariant> preferences_header = {"Field","Value"};

  /*************************
   * PREFERENCES
   *************************/
  TreeItem* preferences_root = new TreeItem ( preferences_header );

  ////////////////
  // DR PROBE
  ////////////////
  QVector<QVariant> preferences_option_1 = {"DrProbe",""};
  TreeItem* preferences_dr_probe = new TreeItem ( preferences_option_1  );
  preferences_root->insertChildren( preferences_dr_probe );


  QVector<QVariant> preferences_option_1_2 = {"celslc execname",_dr_probe_celslc_bin};
  QVector<bool> preferences_option_1_2_edit = {false,true};
  boost::function<bool(std::string)> preferences_option_function_1_2 ( boost::bind( &Settings::set_dr_probe_celslc_bin, this, _1 ) );
  _preferences_dr_probe_celslc = new TreeItem ( preferences_option_1_2 , preferences_option_function_1_2,  preferences_option_1_2_edit  );
  _preferences_dr_probe_celslc->set_item_delegate_type( TreeItem::_delegate_FILE );
  preferences_dr_probe->insertChildren( _preferences_dr_probe_celslc );

  QVector<QVariant> preferences_option_1_3 = {"msa execname",_dr_probe_msa_bin};
  QVector<bool> preferences_option_1_3_edit = {false,true};
  boost::function<bool(std::string)> preferences_option_function_1_3 ( boost::bind( &Settings::set_dr_probe_msa_bin, this, _1 ) );
  _preferences_dr_probe_msa = new TreeItem ( preferences_option_1_3 , preferences_option_function_1_3,  preferences_option_1_3_edit  );
  _preferences_dr_probe_msa->set_item_delegate_type( TreeItem::_delegate_FILE );
  preferences_dr_probe->insertChildren( _preferences_dr_probe_msa );

  QVector<QVariant> preferences_option_1_4 = {"wavimg execname", _dr_probe_wavimg_bin};
  QVector<bool> preferences_option_1_4_edit = {false,true};
  boost::function<bool(std::string)> preferences_option_function_1_4 ( boost::bind( &Settings::set_dr_probe_wavimg_bin, this, _1 ) );
  _preferences_dr_probe_wavimg = new TreeItem ( preferences_option_1_4 , preferences_option_function_1_4,  preferences_option_1_4_edit  );
  _preferences_dr_probe_wavimg->set_item_delegate_type( TreeItem::_delegate_FILE );
  preferences_dr_probe->insertChildren( _preferences_dr_probe_wavimg );

  QVector<QVariant> preferences_option_1_5 = {"im2model-api url", im2model_api_url };
  QVector<bool> preferences_option_1_5_edit = {false,true};
  boost::function<bool(std::string)> preferences_option_function_1_5 ( boost::bind( &Settings::set_im2model_api_url, this, _1 ) );
  _preferences_im2model_api_url = new TreeItem ( preferences_option_1_5 , preferences_option_function_1_5,  preferences_option_1_5_edit  );
  preferences_dr_probe->insertChildren( _preferences_im2model_api_url );

  project_preferences_model = new TreeModel( preferences_root );

  ui->qtree_view_project_project_preferences->setModel( project_preferences_model );
  ui->qtree_view_project_project_preferences->setItemDelegate( _tree_delegate );
  ui->qtree_view_project_project_preferences->expandAll();
  ui->qtree_view_project_project_preferences->setEditTriggers(QAbstractItemView::AllEditTriggers);
}
