<?php

/**
 * @file
 * Widgets for facets rendered as fivestar stars.
 */

/**
 * Widget that renders facets as a series of text links.
 */
class SearchApiFivestarWidgetStars extends FacetapiWidgetLinks {

  /**
   * Renders the stars.
   */
  public function execute() {
    $element = &$this->build[$this->facet['field alias']];
    $settings = $this->settings->settings;
    $fivestar_widgets = module_invoke_all('fivestar_widgets');
    $fivestar_widget_name = $settings['fivestar_widget'];
    $fivestar_widget = array(
      'name' => isset($fivestar_widgets[$fivestar_widget_name]) ? strtolower($fivestar_widgets[$fivestar_widget_name]) : 'default',
      'css' => $fivestar_widget_name,
    );
    $stars = variable_get('search_api_fivestar_rating_stars');

    foreach ($element as $value => &$item) {
      $item['#rating'] = $item['#indexed_value'];
      $item['#widget'] = $fivestar_widget;
      $item['#stars'] = $stars;
      $item['#prefix'] = $settings['widget_prefix'];
      $item['#suffix'] = $settings['widget_suffix'];
    }
    $this->setThemeHooks($element);

    $element = array(
      '#theme' => 'item_list',
      '#items' => $this->buildListItems($element),
      '#attributes' => $this->build['#attributes'],
    );
  }

  /**
   * Transforms the render array for use with theme_item_list().
   */
  function buildListItems($build) {
    $settings = $this->settings->settings;

    // Initializes links attributes, adds rel="nofollow" if configured.
    $attributes = ($settings['nofollow']) ? array('rel' => 'nofollow') : array();
    $attributes += array('class' => $this->getItemClasses());

    // Builds rows.
    $items = array();
    foreach ($build as $value => $item) {
      $row = array('class' => array());

      // Allow adding classes via altering.
      if (isset($item['#class'])) {
        $attributes['class'] = array_merge($attributes['class'], $item['#class']);
      }
      // Initializes variables passed to theme hook.
      $variables = array(
        'text' => $item['#markup'],
        'prefix' => $item['#prefix'],
        'suffix' => $item['#suffix'],
        'path' => $item['#path'],
        'count' => $item['#count'],
        'rating' => $item['#rating'],
        'widget' => $item['#widget'],
        'stars' => $item['#stars'],
        'options' => array(
          'attributes' => $attributes,
          'html' => $item['#html'],
          'query' => $item['#query'],
        ),
      );

      // Adds the facetapi-zero-results class to items that have no results.
      if (!$item['#count']) {
        $variables['options']['attributes']['class'][] = 'facetapi-zero-results';
      }

      // Add an ID to identify this link.
      $variables['options']['attributes']['id'] = drupal_html_id('facetapi-link');

      // If the item has no children, it is a leaf.
      if (empty($item['#item_children'])) {
        $row['class'][] = 'leaf';
      }
      else {
        // If the item is active or the "show_expanded" setting is selected,
        // show this item as expanded so we see its children.
        if ($item['#active'] || !empty($settings['show_expanded'])) {
          $row['class'][] = 'expanded';
          $row['children'] = $this->buildListItems($item['#item_children']);
        }
        else {
          $row['class'][] = 'collapsed';
        }
      }

      // Gets theme hook, adds last minute classes.
      $class = ($item['#active']) ? 'facetapi-active' : 'facetapi-inactive';
      $variables['options']['attributes']['class'][] = $class;

      // Themes the link, adds row to items.
      $row['data'] = theme($item['#theme'], $variables);
      $items[] = $row;
    }

    return $items;
  }

  /**
   * Recursive function that sets each item's theme hook.
   */
  protected function setThemeHooks(array &$build) {
    foreach ($build as $value => &$item) {
      $item['#theme'] = ($item['#active']) ? 'search_api_fivestar_facetapi_link_active' : 'search_api_fivestar_facetapi_link_inactive';
      if (!empty($item['#item_children'])) {
        $this->setThemeHooks($item['#item_children']);
      }
    }
    return $this;
  }

  /**
   * Allows the widget to provide additional settings to the form.
   */
  function settingsForm(&$form, &$form_state) {
    parent::settingsForm($form, $form_state);
    $settings = $this->settings->settings;

    $fivestar_widgets = module_invoke_all('fivestar_widgets');

    $form['widget']['widget_settings']['links'][$this->id]['fivestar_widget'] = array(
      '#type' => 'radios',
      '#title' => t('Star display options'),
      '#description' => t('Choose a style for your widget.'),
      '#options' => array('default' => t('Default')) + $fivestar_widgets,
      '#default_value' => $settings['fivestar_widget'],
      '#attributes' => array('class' => array('fivestar-widgets', 'clearfix')),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['widget_prefix'] = array(
      '#type' => 'textfield',
      '#title' => t('Prefix'),
      '#default_value' => $settings['widget_prefix'],
      '#description' => t('Adds a prefix to the text links.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
    $form['widget']['widget_settings']['links'][$this->id]['widget_suffix'] = array(
      '#type' => 'textfield',
      '#title' => t('Suffix'),
      '#default_value' => $settings['widget_suffix'],
      '#description' => t('Adds a suffix to the text links.'),
      '#states' => array(
        'visible' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
        'enabled' => array(
          'select[name="widget"]' => array('value' => $this->id),
        ),
      ),
    );
  }

  /**
   * Returns defaults for the settings this widget provides.
   */
  function getDefaultSettings() {
    return array(
      'fivestar_widget' => 'default',
      'widget_prefix' => '',
      'widget_suffix' => '',
    );
  }
}
