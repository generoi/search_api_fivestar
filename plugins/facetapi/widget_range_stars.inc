<?php

/**
 * @file
 * Widgets for facets rendered as fivestar stars.
 */

/**
 * Widget that renders facets as a series of text links.
 */
class SearchApiFivestarWidgetRangeStars extends SearchApiFivestarWidgetStars {

  /**
   * Renders the stars.
   */
  public function execute() {
    $element = &$this->build[$this->facet['field alias']];
    $settings = $this->settings->settings;
    $global_settings = $this->facet->getAdapter()->getFacetSettingsGlobal($this->facet->getFacet());
    $fivestar_widgets = module_invoke_all('fivestar_widgets');
    $fivestar_widget_name = $settings['fivestar_widget'];
    $fivestar_widget = array(
      'name' => isset($fivestar_widgets[$fivestar_widget_name]) ? strtolower($fivestar_widgets[$fivestar_widget_name]) : 'default',
      'css' => $fivestar_widget_name,
    );
    $stars = variable_get('search_api_fivestar_rating_stars');

    // Fetch the real star data, and not the range bundles.
    $values = array_keys($element);
    $values = array_filter($values, 'is_numeric');

    // Figure out the max and minimum star rating available.
    sort($values, SORT_NUMERIC);
    $min_all = $values[0];
    $max_all = $values[count($values) - 1];
    $tmp_element = array();
    foreach ($element as $key => $value) {
      // Restructure the range query in case the limit has changed.
      // If another filter has been added, limiting the range with [0 TO 3]
      // instead of the earlier [0 TO 4] we need to modify the key (set to
      // "[0 TO 4]") into the new range.
      if (!is_numeric($key)) {
        list($from, $to) = explode('TO', str_replace(array('[', ']'), '', $key));
        $from = trim($from);
        // @todo for now this only supports the scenario of 0-,1-,2- etc.
        // $from = max($min_all, trim($from));
        $to = min($max_all, trim($to));
        $range = "[$from TO $to]";
        $tmp_element[$range] = $value;
        $tmp_element[$range]['#markup'] = "$from - $to";
        $tmp_element[$range]['#indexed_value'] = $range;
      }
      // Simply copy over the regular elements.
      else {
        $tmp_element[$key] = $value;
      }
    }
    $element = $tmp_element;

    // Create range bundles.
    if (strlen($settings['range_advanced']) > 0) {
      // Get Search API stuff.
      $searcher = $this->facet->getAdapter()->getSearcher();
      $index_id = explode('@', $searcher);
      $index = search_api_index_load($index_id[1]);
      list($query, $results) = $this->facet->getAdapter()->getCurrentSearch();

      // Prepare variables for min/max query.
      $variables = array(
        'element' => $element,
        'index' => $index,
        'range_field' => $this->facet['field alias'],
        'target' => $this->facet->getAdapter()->getSearchPath(),
        'query' => $query,
        'prefix' => '',
        'suffix' => '',
      );
      $element = search_api_ranges_generate_ranges_advanced($variables, $settings['range_advanced']);
    }

    foreach ($element as $value => &$item) {
      // It's a range bundle
      if (strpos($value, '-') !== FALSE) {
        list($from, $to) = explode('-', $value);
        $from = (int) $from;
        $to = (int) $to;
      }
      // Regular item
      else {
        $from = (int) $value;
      }

      $item['#rating'] = $from;
      $item['#widget'] = $fivestar_widget;
      $item['#stars'] = $stars;
      $item['#prefix'] = $settings['widget_prefix'];
      $item['#suffix'] = $settings['widget_suffix'];
      // #active is lost somewhere, reapply it.
      if (!empty($tmp_element["[$from TO $to]"]['#active'])) {
        $item['#active'] = 1;
      }

      // honor active item limit
      if ($global_settings->settings['limit_active_items']) {
        // remove all earlier queries for this facet.
        foreach ($item['#query']['f'] as $idx => $filter) {
          if (0 === strpos($filter, $this->facet['field alias'])) {
            unset($item['#query']['f'][$idx]);
          }
        }
        // unless active (when the query should be removed) reapply the current
        // items query.
        if (!$item['#active']) {
          $filter = isset($to) ? "[$from TO $to]" : $from;
          $item['#query']['f'][] = urlencode($this->facet['field alias']) . ':' . $filter;
        }
      }
    }
    $this->setThemeHooks($element);

    $element = array(
      '#theme' => 'item_list',
      '#items' => $this->buildListItems($element),
      '#attributes' => $this->build['#attributes'],
    );
  }

  /**
   * Allows the widget to provide additional settings to the form.
   */
  function settingsForm(&$form, &$form_state) {
    parent::settingsForm($form, $form_state);
    $settings = $this->settings->settings;

    $form['widget']['widget_settings']['links'][$this->id]['range_advanced'] = array(
      '#type' => 'textarea',
      '#title' => t('Advanced range'),
      '#default_value' => $this->settings->settings['range_advanced'],
      '#description' => t('Add a pre-defined list of ranges to create
        "range facets". For examples, refer to README in search api range.'),
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
    return parent::getDefaultSettings() + array('range_advanced' => '');
  }
}
