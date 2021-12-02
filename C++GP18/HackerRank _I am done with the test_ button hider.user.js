// ==UserScript==
// @name         HackerRank "I am done with the test" button hider
// @namespace    https://www.nng.com/grandprix/
// @version      0.1
// @author       David Hanak <david.hanak@nng.com>
// @match        https://www.hackerrank.com/tests/7rpbr24ipmp/*
// @grant        none
// ==/UserScript==

(function() {
    'use strict';

    console.log('Hiding "I am done with the test" button...');
    $('head').append('<style type="text/css">button.test-done { display: none; }</style>');
})();