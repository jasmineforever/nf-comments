$(function(){
    $("head").append("<link>");
    css = $("head").children(":last");
    css.attr({
        rel: "stylesheet",
        type: "text/css",
        href: "../css/nf_comments.css"
    });
})
//****************************************************************************************
//get config
//****************************************************************************************
var scripts = document.getElementsByTagName('script');
var src = document.getElementsByTagName('script')[scripts.length - 1].src;
var arg = src.indexOf('?') !== -1 ? src.split('?').pop() : '';
var nf_settings = {};
arg.replace(/(\w+)(?:=([^&]*))?/g, function(a, key, value) {

    nf_settings[key] = value;
});
//****************************************************************************************
//Global variables
//****************************************************************************************
var comments_url = (nf_settings.comments_url !== undefined) ? nf_settings.comments_url : "";
if(0 != comments_url.search(/^https?:\/\//i))
{
    comments_url = "http://" +comments_url;
}
var nf_key = window.location.pathname;
var nf_page_max = 0;
var nf_now_page = 1;
var nf_pagination = new NFPagination( function(index) {

    nf_now_page = index;
    getNFComments(nf_now_page, false);
});

//****************************************************************************************
//get comments
//****************************************************************************************
function getNFComments(pageIndex, needGotoEnd) {

    $.ajax(comments_url +'?key='+ nf_key +'&page=' + pageIndex, {
        dataType: 'json'
    }).done(function (data) {
        // console.log('成功, 收到的数据: ' + JSON.stringify(data));
        //  创建分页和显示

        if(data.code != 0)
        {
            console.log("err get nf comments:" + data.msg);
            return;
        }
        if (data.max_page == undefined)
        {
            console.log("undefined comments");
            return;
        }
        var now_time = parseInt(new Date().getTime() / 1000);
        if(data.max_page > 0)
        {
            for(var i = 0; i < data.comments.length; i++)
            {
                for (var j = 0; j < data.comments[i].length; j++)
                {
                    data.comments[i][j].time = getNFTimeStr(now_time - data.comments[i][j].time);
                    if(data.comments[i][j].website.length > 4)
                    {
                        if(0 != data.comments[i][j].website.search(/^https?:\/\//i))
                        {
                            data.comments[i][j].website = "http://" + data.comments[i][j].website;
                        }

                    }
                }
            }
            for(var i = 0; i < data.comments.length; i++)
            {
                for(var j = 0; j < data.comments[i].length; j++)
                {
                    if(data.comments[i][j].name == "$-")
                    {
                        data.comments[i][j].name = "火星网友";
                    }
                }
            }
        }
        var nf_commens_out = template({comments_array: data});
        $(".nf-comments-main").empty();
        $(".nf-comments-main").append(nf_commens_out);
        if(data.max_page > 0)
        {
            for(var i = 0; i < data.comments.length; i++)
            {
                for(var j = 0; j < data.comments[i].length; j++)
                {
                    var avatar_id = "#nf_avatar_" + data.comments[i][j].comment_id;
                    if(data.comments[i][j].is_owner)
                    {
                        $(avatar_id).append('<img class="nf-avatar" src="' +data.owner_avatar + '">');
                    }
                    else
                    {
                        var avatar_data = new Identicon(md5(data.comments[i][j].name), { size: 42, format: 'svg'}).toString();
                        $(avatar_id).append('<img class="nf-avatar" src="data:image/svg+xml;base64,' + avatar_data + '">');
                    }
                }
            }
        }
        var name = getCookie("nf_user_name");
        if(name.length > 0)
        {
            $(".nf-remember-me-checkbox").prop('checked', true);
            $(".nf-info-name-input").val(name);
            $(".nf-info-email-input").val(getCookie("nf_user_email"));
            $(".nf-info-website-input").val(getCookie("nf_user_website"));
        }
        else
        {
            $(".nf-remember-me-checkbox").prop('checked', false);
        }
        $(".nf-remember-me-checkbox").change(function(){

            if($(this).is(':checked') == false)
            {
                delCookie("nf_user_name");
                delCookie("nf_user_email");
                delCookie("nf_user_website");
                $(".nf-remember-me-checkbox").prop('checked', false);
            }
            else
            {
                $(".nf-remember-me-checkbox").prop('checked', true);
            }
        });
        $(".nf-reply-button").click(function(){
            arr = $(this).attr('id').split("_");

            if(arr.length == 4)
            {
                if ($("#nf_comment_area_" + arr[3]).is(':hidden'))
                {
                    $(this).text('关闭');
                }
                else
                {
                    $(this).text('回复');
                }
                $("#nf_comment_area_" + arr[3]).toggle();
            }
        });
        $(".nf-content-input").bind('propertychange input', function () {

            arr = $(this).attr('id').split("_");

            if(arr.length == 4)
            {
                $("#nf_content_input_count_" +  arr[3]).text($(this).val().length);
            }
        });

        $(".nf-submit-button").click(function () {

            var arr = $(this).attr('id').split("_");

            if(arr.length == 5)
            {
                var email_str =$("#nf_email_input_" + arr[3]).val();
                var content_str = $("#nf_content_input_" + arr[3]).val();
                var name_str =$("#nf_name_input_" + arr[3]).val();
                var website_str =$("#nf_website_input_" + arr[3]).val();
                var reg = /^[\w-]+(\.[\w-]+)*@[\w-]+(\.[\w-]+)+$/;
                var tips_id_str = "#nf_error_tips_" +  arr[3];
                if(content_str.length <= 0)
                {
                    $(tips_id_str).text("说点什么吧！");
                    $(tips_id_str).fadeIn(200, function(){ $(tips_id_str).delay(600).fadeOut(200);});
                    return;
                }
                else if(content_str.length > 255)
                {
                    $(tips_id_str).text("你的话太多了");
                    $(tips_id_str).fadeIn(200, function(){ $(tips_id_str).delay(600).fadeOut(200);});
                    return;
                }
                if(name_str.indexOf(" ") >= 0 || name_str.indexOf("\t") >= 0)
                {
                    $(tips_id_str).text("名字中不能包含空格");
                    $(tips_id_str).fadeIn(200, function(){ $(tips_id_str).delay(600).fadeOut(200);});
                    return;
                }
                if(name_str.length > 30)
                {
                    $(tips_id_str).text("你的名字太长啦！");
                    $(tips_id_str).fadeIn(200, function(){ $(tips_id_str).delay(600).fadeOut(200);});
                    return;
                }
                if(email_str.length > 0 && (email_str.length > 50 || !reg.test(email_str)))
                {
                    $(tips_id_str).text("邮箱格式错误！");
                    $(tips_id_str).fadeIn(200, function(){ $(tips_id_str).delay(600).fadeOut(200);});
                    return ;
                }

                reg = /^(?=^.{3,255}$)(http(s)?:\/\/)?(www\.)?[a-zA-Z0-9][-a-zA-Z0-9]{0,62}(\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})+(:\d+)*(\/\w+\.\w+)*$/;

                if(website_str.length > 0 && (website_str.length > 80 || !reg.test(website_str)))
                {
                    $(tips_id_str).text("网站格式错误！");
                    $(tips_id_str).fadeIn(200, function(){ $(tips_id_str).delay(600).fadeOut(200);});
                    return ;
                }
                pushNFComments(name_str, email_str, website_str, content_str, parseInt(arr[3]), parseInt(arr[4]));

                if($("#nf_remember_me_checkbox_0").is(":checked") && name_str.length > 0)
                {
                    if(getCookie("nf_user_name") != name_str)
                    {
                        setCookie("nf_user_name", name_str, 365);
                    }

                    if(getCookie("nf_user_email") != email_str)
                    {
                        setCookie("nf_user_email", email_str, 365);
                    }

                    if(getCookie("nf_user_website") != website_str)
                    {
                        setCookie("nf_user_name", website_str, 365);
                    }
                }
            }
        });
        if(data.max_page != nf_page_max)
        {
            nf_pagination.createPagination(data.comments_count_per_page, data.max_page);
            nf_page_max = data.max_page;

            if(needGotoEnd)
            {
                getNFComments(nf_page_max, false);
                nf_pagination.gotoRealPage(nf_page_max);
                document.getElementById($('.nf-reply-button:last').attr("id")).scrollIntoView();
            }
        }
        else
        {
            if(needGotoEnd)
            {
                nf_pagination.gotoRealPage(nf_page_max);
                document.getElementById($('.nf-reply-button:last').attr("id")).scrollIntoView();
            }
        }

    }).fail(function (xhr, status) {

        console.warn('failed to get comments: ' + xhr.status + ', reason: ' + status);

    }).always(function () {

    });
}
//****************************************************************************************
//push comments
//****************************************************************************************
function pushNFComments(_name, _email, _website, _content, _reply_to_id, _floor_id)
{
    $.ajax(comments_url, {
        dataType: 'json',
        method: 'POST',
        data:  JSON.stringify({key: nf_key, name: _name, email: _email,
            website: _website, content: _content, reply_to_id: _reply_to_id, floor_id: _floor_id})

    }).done(function (data) {

        if(_reply_to_id == 0)
        {
            getNFComments(nf_page_max, true);
        }
        else
        {
            getNFComments(nf_pagination.getNowRealIndex(), false);
        }

    }).fail(function (xhr, status) {

        console.warn('failed to push comments: ' + xhr.status + ', reason: ' + status);

    }).always(function () {

    });
}
//****************************************************************************************
//event
//****************************************************************************************
$(document).ready(function() {

    getNFComments(1);
});
//****************************************************************************************
// utils function
//****************************************************************************************
function getCookie(objName) {
    var arrStr = document.cookie.split("; ");
    for (var i = 0; i < arrStr.length; i++) {
        var temp = arrStr[i].split("=");
        if (temp[0] == objName) return unescape(temp[1]);
    }
    return "";
}
function setCookie(cname, cvalue, exdays) {
    var d = new Date();
    console.log(cname + '/' + cvalue + '/' + exdays)
    d.setTime(d.getTime() + (exdays * 24 * 60 * 60 * 1000));
    var expires = "expires=" + d.toGMTString();
    document.cookie = cname + "=" + escape(cvalue) + "; " + expires;
    //console.log(document.cookie);
}
function delCookie(name) {
    var exp = new Date();
    exp.setTime(exp.getTime() - 60 * 60 * 1000);
    var cval = getCookie(name);
    if (cval != null)
        document.cookie = name + "=" + cval + ";expires=" + exp.toGMTString() + ";path=/";
}
function getNFTimeStr(seconds) {

    var ret_str = "很久以前";
    seconds= parseInt(seconds);
    if(seconds >= 0)
    {
        if(seconds < 60)
        {
            ret_str = seconds + "秒前";
        }
        else if( seconds < 60 * 60)
        {
            ret_str = parseInt(seconds / 60) + "分钟前";
        }
        else if( seconds < 60 * 60 * 24)
        {
            ret_str = parseInt(seconds / (60 * 60)) + "小时前";
        }
        else if( seconds < 60 * 60 * 24 * 30)
        {
            ret_str = parseInt(seconds / (60 * 60 * 24)) + "天前";
        }
        else if( seconds < 60 * 60 * 24 * 365)
        {
            ret_str = parseInt(seconds / (60 * 60 * 24 * 30)) + "个月前";
        }
        else
        {
            ret_str = parseInt(seconds / (60 * 60 * 24 * 365)) + "年前";
        }
    }
    return ret_str;
}
