<?php

namespace dfd\Twig;

use Drupal\Core\Template\TwigEnvironment;
use Twig_Lexer;
use Twig_LexerInterface;

class Lexer extends Twig_Lexer implements Twig_LexerInterface {

  function tokenize($source, $filename = NULL) {
    $code = $source->getCode();
    $code = str_replace(["\r\n", "\r"], "\n", $code);
    $code = $this->earlyRender($code);

    return parent::tokenize(new \Twig_Source($code, $source->getName(), $source->getPath()), $filename);
  }

  protected function earlyRender(string $code): string {
    if (strpos($code, '{% early %}') === FALSE) {
      return $code;
    }
    $code = preg_replace('/{% ((end)?early) %}/', '<!-- $1 -->', $code);
    /** @var TwigEnvironment $twig */
    $twig = \Drupal::service('twig');
    $rendered = $twig->renderInline($code);
    preg_match_all('/<!-- early -->.*?<!-- endearly -->/s', $code, $orig_match);
    preg_match_all('/<!-- early -->(.*?)<!-- endearly -->/s', $rendered, $render_match);

    return str_replace($orig_match[0], $render_match[1], $code);
  }

}
