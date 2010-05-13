<?php
// $Id: FeedsCSVParser.inc,v 1.8 2010/03/29 04:02:37 alexb Exp $

/**
 * Parses a given file as a CSV file.
 */
class MailhandlerParser extends FeedsParser {

  /**
   * Implementation of FeedsParser::parse().
   */
  public function parse(FeedsImportBatch $batch, FeedsSource $source) {
    $fetched = $batch->getRaw();
    if ($fetched['new']) {
      module_load_include('inc', 'mailhandler', 'mailhandler.retrieve');
      // TODO implement handling $limit.  This could be done very easily
      // w/ a setting in this->config, and passed to mailhandler_retrieve()
      $messages = array();
      $messages = mailhandler_retrieve($fetched['mailbox']);
      // Populate batch.
      if (!empty($messages)) {
        foreach ($messages as &$message) {
          $this->parseExtensions($message);
        }
        dwd('messages', $messages);
        $batch->setItems($messages);
      }
    }
    else {
      if (isset($fetched['new'])) {
        drupal_set_message('No new messages.');
      }
    }
  }

  /*
   * Set known sources and parse additional sources from body.
   */
  public function parseExtensions(&$message) {    
    // Populate $message with all values from 'header' object.
    $parts = (array) $message['header'];
    foreach ($parts as $key => $value) {
      // Some keys are already taken, so do not overwrite them.
      if (!in_array($key, array('header', 'origbody', 'mimeparts', 'mailbox'))) {
        $message[$key] = $value;
      }
    }
    // Custom cases.  TODO: There's probably a smarter way to do this.
    //$message['type'] = $this->config['content_type'];
    $message['title'] = $message['subject'];
    $message['body'] = $message['origbody'];
    if ($this->config['commands']) {
      $message['commands'] = $this->config['default_commands'];
    }
  }
  /*
   * This defines sources which user's can select to map values to.
   */
  public function getMappingSources() {
    // TODO: define the rest of the elements that IMAP PHP provides
    $sources = array(
      'title' => array(
        'name' => t('Subject'),
        'description' => t('The email subject.'),
      ),
      'body' => array(
        'name' => t('Body'),
        'description' => t('The cleaned email body, commands removed.'),
      ),
      'type' => array(
        'name' => t('Type'),
        'description' => t('The type of node'),
      ),
      'size' => array(
        'name' => t('Size'),
        'description' => t('The size of the mail'),
      ),
      'origbody' => array(
        'name' => t('Original body'),
        'description' => t('The original body of the email'),
      ),
    );
    drupal_alter('feeds_node_processor_sources', $sources);
    return $sources;
  }

  /**
   * Override parent::configForm().
   */
  public function configForm(&$form_state) {
    $form = array();
    $form['commands'] = array(
      '#type' => 'checkbox',
      '#title' => t('Parse commands?'),
      '#description' => t('Check this box to allow commands sent by users in email messages to be parsed and applied to the created node.'),
      '#options' => array(0 => t('On'), 1 => t('Off')),
      '#default_value' => $this->config['commands'],
    );
    $form['default_commands'] = array(
      '#type' => 'textarea',
      '#title' => t('Default commands'),
      '#description' => t('Default commands for this mailbox feed.'),
      '#default_value' => $this->config['default_commands'],
    );
    return $form;
  }
  
  /**
   * Override parent::configDefaults().
   */
  public function configDefaults() {
    return array(
      'commands' => 1,
      'default_commands' => '',
    );
  }

}