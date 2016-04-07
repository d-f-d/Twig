<?php
namespace dfd\Twig\BEM;

use Twig_Token;
use Twig_TokenParser;

class TokenParser extends Twig_TokenParser {
  private $tag;
  private $type;

  function __construct($tag = 'b', $type = 'block') {
    $this->tag = $tag;
    $this->type = $type;
  }

  public function parse(Twig_Token $token) {
    $parser = $this->parser;
    $stream = $parser->getStream();

    if ('block' === $this->type && $stream->nextIf(Twig_Token::BLOCK_END_TYPE)) {
      return new Node([], ['method' => 'block'], $token->getLine(), $this->getTag());
    }

    if ('block' === $this->type && $stream->nextIf(Twig_Token::OPERATOR_TYPE, '=')) {
      $block = $parser->getExpressionParser()->parseExpression();
      $stream->expect(Twig_Token::BLOCK_END_TYPE);
      return new Node(['block' => $block], ['method' => 'setBlock'], $token->getLine(), $this->getTag());
    }

    if ('block' === $this->type) {
      $more_classes = $parser->getExpressionParser()->parseExpression();
      $stream->expect(Twig_Token::BLOCK_END_TYPE);
      return new Node(['more' => $more_classes], ['method' => 'blockMore'], $token->getLine(), $this->getTag());
    }


    if ('element' === $this->type) {
      $element = $parser->getExpressionParser()->parseExpression();
      if ($stream->nextIf(Twig_Token::BLOCK_END_TYPE)) {
        return new Node(['element' => $element], ['method' => 'element'], $token->getLine(), $this->getTag());
      }

      if ($stream->nextIf(Twig_Token::PUNCTUATION_TYPE, ',')) {
        $mod = $parser->getExpressionParser()->parseExpression();
        if ($stream->nextIf(Twig_Token::BLOCK_END_TYPE)) {
          return new Node([
            'element' => $element,
            'mod' => $mod
          ], ['method' => 'elementMod'], $token->getLine(), $this->getTag());
        }
        elseif ($stream->nextIf(Twig_Token::PUNCTUATION_TYPE, ',')) {
          $more_classes = $parser->getExpressionParser()->parseExpression();
          $stream->expect(Twig_Token::BLOCK_END_TYPE);
          return new Node([
            'element' => $element,
            'mod' => $mod,
            'more' => $more_classes
          ], ['method' => 'elementModMore'], $token->getLine(), $this->getTag());
        }
      }
    }
  }

  public function getTag() {
    return $this->tag;
  }
}