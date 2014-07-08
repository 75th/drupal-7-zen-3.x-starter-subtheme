(function($) {

	var methods = {
		init: function(params) {
			$(this).hide();
			var multislideID = params['multislideID'];
			var region = params['region'];
			
			var data = params['data'];
			data = data['nodes'];
			
			var slideType = typeof params['type'] !== 'undefined' ? params['type'] : 'bg';
			var hasContent = typeof params['hasContent'] !== 'undefined' ? params['hasContent'] : false;
			var withFrame = typeof params['withFrame'] === 'boolean' ? params['withFrame'] : false;
			var switcher = typeof params['switcher'] !== 'undefined' ? params['switcher'] : false;
			var newWindow = typeof params['newWindow'] !== 'undefined' ? params['newWindow'] : false;
		//	console.log(multislideID + ' ' + region + ' ' + withFrame + ' (' + typeof withFrame + ')');

			$(this).each(function() {

				$(this).addClass('multislide');
 				$(this).addClass('multislide-' + multislideID);

 				
				
				if(withFrame) {
					var frameElement = '';

					frameElement += '<div class="controls"><ul>';

					for(var i = 1; i <= data.length; i++) { // Goto function is 1-indexed
						frameElement += '<li><a class="control-pager control-goto-' + i + '"><span>Go to slide	' + String(i) + '</span></a></li>';
					}

					/*

					frameElement += '<a class="control-prev"><span>Previous</span></a>';
					frameElement += '<a class="control-next"><span>Next</span></a>';

					*/
					frameElement += '</ul></div>';
					$(this).append(frameElement);

					$(this).find('.control-prev').click(function() {
						clearInterval(switcher);
						$('.multislide-' + multislideID).multislide('prev');
					})

					$(this).find('.control-next').click(function() {
						clearInterval(switcher);
						$('.multislide-' + multislideID).multislide('next');
					})

					$(this).find('.control-pager').each(function(index) {
						$(this).on('click', function() {
							clearInterval(switcher);
							if(!$(this).hasClass('active')) {
								$('.multislide-' + multislideID).multislide('goto', (index + 1));
							}
						});
					});
				}

				
				
				$(this).append('<div class="slides"></div>');

				$(this).find('.slides').each(function() {

					for(var i = 0; i < data.length; i++) {
						var node = data[i]['node'];
						var filePath = node[region];

						var captionParts = [ node['caption_line_1'], node['caption_line_2'], node['caption_line_3'] ]

						var caption = '';

						for(var j = 0; j < captionParts.length; j++) {
							if(captionParts[j]) {
								caption += '<span class="line-' + String(j + 1) + '">' + captionParts[j] + '</span>';
							}
						}
		
						var elementType = 'div';
						var classes = 'slide-' + i;

						var attrs = '';
						var content = '';

						if(typeof node['link'] !== 'undefined') {
							elementType = 'a';
							attrs += ' href="' + node['link'] + '"';

							if(newWindow) {
								attrs += ' target="_blank"';
							}
						}

						if(slideType === 'bg') {
							attrs += ' style="background-image: url(\'' + filePath + '\');"'
							content = '<div class="caption">' + caption + '</div>';
						} else if(slideType === 'img') {
							content = '<img src="' + filePath + '">';
						}

						attrs += ' class="' + classes + '"';
											
						var element = '<' + elementType + attrs + '>' + content + '</' + elementType + '>';
						$(this).append(element);

					}
					$(this).children(':first-child').addClass('active');
				});

				$(this).find('.controls a').eq(0).addClass('active');

				$(this).fadeIn(300);
			});
		},

		goto: function(slide) {
			console.log(slide);
			$(this).each(function() {
				$(this).children('.slides').each(function() {
					var $activeSlide = $(this).children('.active');

					var $target = $(this).children().eq(slide - 1);

					if(!$target) {
					//	console.log('Invalid target');
						return false;
					}

					if($activeSlide === $target) {
					//	console.log('That’s the current slide')
						return false;
					}
					$activeSlide.addClass('almost-inactive')
					$target.addClass('almost-active').show();

					if(Modernizr.csstransitions) {
						setTimeout(function() {
							$activeSlide.removeClass('active almost-inactive');
							$target.addClass('active').removeClass('almost-active');
						}, 1300);
					} else {
						$activeSlide.fadeOut(1000, function() {
							$activeSlide.removeClass('active almost-inactive');
							$target.addClass('active').removeClass('almost-active');
						});
					}

					
				});

				$controls = $(this).find('.controls a');

				$controls.removeClass('active');
				$controls.eq(slide - 1).addClass('active');
			});
		},

		next: function() {
			$(this).each(function() {

				$(this).children('.slides').each(function() {
					var $activeSlide = $(this).children('.active');

					var $next = $activeSlide.next();

					if($next.size() == 0) {
						$next = $(this).children(':first-child');
					}

					var nextSlide = $next.index() + 1;
				
					$(this).parent().multislide('goto', nextSlide);
				});
				
				
			});
		},

		prev: function() {
			$(this).each(function() {
			
				$(this).children('.slides').each(function() {
					var $activeSlide = $(this).children('.active');

					var $prev = $activeSlide.prev();

					if($prev.size() == 0) {
						$prev = $(this).children(':last-child');
					}

					var prevSlide = $prev.index() + 1;

					$(this).parent().multislide('goto', prevSlide);
				});				
			});
		}
	};

	$.fn.multislide = function( method ) {

		// Method calling logic
		if ( methods[method] ) {
			return methods[ method ].apply( this, Array.prototype.slice.call( arguments, 1 ));
		} else if ( typeof method === 'object' || ! method ) {
			return methods.init.apply( this, arguments );
		} else {
			$.error( 'Method ' +  method + ' does not exist in multislide' );
		}    

	};



	$.fn.preload = function() {
	    this.each(function(){
	        $('<img/>')[0].src = this;
	    });
	}

})(jQuery);