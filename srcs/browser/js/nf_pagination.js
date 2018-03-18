(function(){
    //****************************************************************************************
    //NFPagination constructor
    //****************************************************************************************
    var NFPagination = function(_pageSelectCallback) {

        this.pageSelected = _pageSelectCallback;
    }
    //****************************************************************************************
    //prototype
    //****************************************************************************************
    NFPagination.prototype = {

        now_page_index: 1,
        page_max: 0,
        more_page: 0,
        one_page_size: 5,
        pageSelected: null,
        //****************************************************************************************
        //create Pagination
        //****************************************************************************************
        createPagination: function(comments_count_per_page, _max_page) {

            var handler = this;
            this.one_page_size = comments_count_per_page;
            $(".nf-pagination").empty();
            if(_max_page > 1)
            {
                this.now_page_index = 1;
                this.page_max = _max_page;
                $(".nf-pagination").append("<div class=\"nf-page-flip-button nf-page-pre-button\" id=\"nf_page_pre_button\">上一页</div>");
                $(".nf-pagination").append("<div class=\"nf-page-index-button\" id=\"nf_page_index_button_left\">...</div>");
                for(var i = 1; i <= this.one_page_size; i++)
                {
                    $(".nf-pagination").append("<div class=\"nf-page-index-button\" id=\"nf_page_index_button_"+ i + "\">"+ i + "</div>");
                }
                $(".nf-pagination").append("<div class=\"nf-page-index-button\" id=\"nf_page_index_button_right\">...</div>");
                $(".nf-pagination").append("<div class=\"nf-page-flip-button nf-page-next-button\" id=\"nf_page_next_button\">下一页</div>");
                $("#nf_page_pre_button").hide();
                $("#nf_page_index_button_left").hide();

                if(this.page_max <= this.one_page_size)
                {
                    $("#nf_page_index_button_right").hide();
                }
                if(this.page_max < this.one_page_size)
                {
                    for( var i = this.page_max + 1; i <= this.one_page_size; i++)
                    {
                        $("#nf_page_index_button_" + i).hide();
                    }
                }
                //action
                $("#nf_page_index_button_right").on("click", function() {

                    handler.more_page += 1;
                    handler.gotoPage(1);
                    handler.doSelectCallback();
                });

                $("#nf_page_index_button_left").on("click", function() {

                    handler.more_page -= 1;
                    handler.gotoPage(handler.one_page_size);
                    handler.doSelectCallback();
                });

                $(".nf-page-index-button").on("click", function() {

                    var id =$(this).attr("id");
                    if(id == "nf_page_index_button_right" || id == "nf_page_index_button_left")
                    {
                        return;
                    }
                    var arr = id.split("_");
                    if(arr.length == 5)
                    {
                        handler.gotoPage((parseInt(arr[4])));
                        handler.doSelectCallback();
                    }
                });
                $("#nf_page_next_button").on("click", function() {

                    var index = 0;
                    if(handler.now_page_index == handler.one_page_size)
                    {
                        index = 1;
                        handler.more_page += 1;
                    }
                    else
                    {
                        index = handler.now_page_index + 1;
                    }
                    handler.gotoPage(index);
                    handler.doSelectCallback();
                });

                $("#nf_page_pre_button").on("click", function() {

                    var index = 0;
                    if(handler.now_page_index == 1)
                    {
                        handler.more_page -= 1;
                        index = handler.one_page_size;
                    }
                    else
                    {
                        index = handler.now_page_index - 1;
                    }
                    handler.gotoPage(index);
                    handler.doSelectCallback();
                });
                $("#nf_page_index_button_1").removeClass("nf-page-index-button");
                $("#nf_page_index_button_1").addClass("nf-page-button-selected");
            }
        },
        gotoRealPage: function(realIndex) {

            if(realIndex <= 0 || realIndex > this.page_max)
            {
                return;
            }
            this.more_page = parseInt((realIndex - 1) / this.one_page_size);
            this.gotoPage((realIndex - 1) % this.one_page_size + 1);
        },
        //****************************************************************************************
        //goto Page
        //****************************************************************************************
        gotoPage: function(pageIndex) {

            if(pageIndex <= 0 || pageIndex > this.one_page_size)
            {
                return ;
            }
            if(this.more_page > 0)
            {
                $("#nf_page_index_button_left").show();
            }
            else
            {
                $("#nf_page_index_button_left").hide();
            }
            if((this.more_page + 1) * this.one_page_size < this.page_max)
            {
                $("#nf_page_index_button_right").show();
            }
            else
            {
                $("#nf_page_index_button_right").hide();
            }

            if(pageIndex + this.more_page * this.one_page_size == this.page_max)
            {
                $("#nf_page_next_button").hide();
            }
            else
            {
                $("#nf_page_next_button").show();
            }

            if(pageIndex + this.more_page * this.one_page_size == 1)
            {
                $("#nf_page_pre_button").hide();
            }
            else
            {
                $("#nf_page_pre_button").show();
            }

            $("#nf_page_index_button_" + this.now_page_index).removeClass("nf-page-button-selected");
            $("#nf_page_index_button_" + this.now_page_index).addClass("nf-page-index-button");

            var hidden_count = 0;
            if(this.page_max - this.more_page * this.one_page_size < this.one_page_size)
            {
                hidden_count = this.one_page_size - (this.page_max % this.one_page_size);
            }
            for(var i = 1; i <= this.one_page_size - hidden_count; ++i)
            {
                $("#nf_page_index_button_" + i).show();
                $("#nf_page_index_button_" + i).text(i + this.more_page * this.one_page_size);
            }
            for(var i = 1 + this.one_page_size - hidden_count; i <= this.one_page_size; ++i)
            {
                $("#nf_page_index_button_" + i).hide();
            }
            this.now_page_index = pageIndex;
            $("#nf_page_index_button_" + this.now_page_index).removeClass("nf-page-index-button");
            $("#nf_page_index_button_" + this.now_page_index).addClass("nf-page-button-selected");
        },
        //****************************************************************************************
        //getNowRealIndex
        //****************************************************************************************
        getNowRealIndex: function() {

            return this.one_page_size * this.more_page + this.now_page_index;
        },
        //****************************************************************************************
        //do select callback
        //****************************************************************************************
        doSelectCallback: function() {

            if(this.pageSelected != null)
            {
                this.pageSelected(this.now_page_index + this.more_page * this.one_page_size);
            }
        }
    }
    if (typeof module !== 'undefined' && typeof module.exports !== 'undefined') {

        module.exports = NFPagination;

    } else {
        window.NFPagination = NFPagination;
    }
})()