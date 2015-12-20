<?php
namespace d\Twig;

use Twig_Extension;
use Twig_SimpleFilter;
use Twig_SimpleFunction;

class Extension extends Twig_Extension {
  public function getFilters() {
    return [
      new Twig_SimpleFilter('check_markup', 'check_markup', ['is_safe' => ['html']]),
      new Twig_SimpleFilter('text_summary', 'text_summary', ['is_safe' => ['html']]),
    ];
  }

  public function getFunctions() {
    return [
      new Twig_SimpleFunction('render_fragment', [$this, 'renderFragment'], ['is_safe' => ['html']]),
      new Twig_SimpleFunction('render_fragment_deferred', [$this, 'renderFragmentDeferred'], ['is_safe' => ['html']]),
    ];
  }

  public function renderFragment($class, $context = []) {
    if ((new \ReflectionClass($class))->implementsInterface('Drupal\\Core\\Render\\RenderableInterface')) {
      $args = func_get_args();
      array_shift($args);
      return (new \ReflectionClass($class))->newInstanceArgs($args);
    }
    return '';
  }

  public function renderFragmentDeferred($class) {
    if ((new \ReflectionClass($class))->implementsInterface('Drupal\\Core\\Render\\RenderableInterface')) {
      $args = func_get_args();
      array_shift($args);
      return [
        '#lazy_builder'       => [$class . '::deferred', $args],
        '#create_placeholder' => TRUE,
      ];
    }
    return '';
  }

  public function getName() {
    return __CLASS__;
  }
}
