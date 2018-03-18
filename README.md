# nf-comments

nf-comments 是一个使用C++实现的轻量，易用，前后端分离的评论系统。

想试一下？请戳 [Live Demo](https://nifengz.com/test/test_comments.html "在线演示")

放一张截图，浏览器端样式参考了 [posativ](https://posativ.org/isso/ "Google") ：

![screenshot](./doc/screenshot.png)

## 目录

- [服务端编译](#服务器端编译)
- [服务端程序使用说明](#服务端程序使用说明)
- [浏览器端使用说明](#浏览器端使用说明)

## 服务器端编译

nf-comments 服务端使用CMake构建，请先安装[CMake](https://cmake.org/ "cmake"). 然后使用如下方式编译：

```bash
cd nf_comments
mkdir build
cd build
cmake .. 
# It will take several minutes to compile dependent libraries 
# default install directory is nf_comments/output.
# you can use 'cmake -DINSTALL_PREFIX=/your_dir ..'to specify the installation directory
make install 
```

编译生成的可执行文件和配置文件默认会安装到项目中的output目录下，如果需要指定安装目录，请使用

```bash
cd nf_comments
mkdir build
cd build
cmake -DINSTALL_PREFIX=/your_dir ..
make install 
```

## 服务端程序使用说明

nf-comments的服务端程序启动依赖一个配置文件，程序安装后，一个默认的配置文件将会和可执行程序一起拷贝到指定的安装目录，启动程序时需要指定一个配置文件：

```bash
./nf_comments -c your_cfg.json
```

如果不指定配置文件，程序将使用当前目录下的nf_config.json作为配置文件。

一个配置文件包含的配置内容如下：

```json
{
  "service_setting":
  {
    "db_path": "./comments.db",
    "db_service_port": 5000,
    "service_uri": "/comments",
    "db_cache_size": 100,
    "allow_origin": "*"
  },

  "comments_setting":
  {
    "enable_pagination": true,
    "max_comments_per_page": 5,
    "max_comment_char_num": 255
  },

  "blog_setting":
  {
    "owner_name": "xxxx",
    "owner_email": "xxx@xxx.com",
    "owner_avatar":"http://xxx.xxx.xxx/avatar.jpg",
    "owner_show_name": "博主"
  }
}
```

配置说明：

| 设置大类         | 子项                  | 类型    | 描述                                                         |
| :--------------- | :-------------------- | :------ | :----------------------------------------------------------- |
| service_setting  | -                     | Object  | 服务相关配置                                                 |
|                  | db_path               | String  | 数据库路径，数据库将在这个路径上创建                         |
|                  | db_service_port       | Integer | 评论服务对外服务的端口号                                     |
|                  | service_uri           | String  | 评论服务对外服务的uri                                        |
|                  | db_cache_size         | Integer | 数据库的查询结果会缓存到内存中，以加快访问速度。这个字段设置缓存占用内存的最大size，单位是MB。 |
|                  | allow_origin          | String  | 如果使用本服务直接对外服务，需要将这个字段设置为您的域名或者*。否则浏览器将因为跨站请求而拒绝访问。如果是使用nginx等代理访问服务请将该字段设置为空，然后自行在代理中添加跨站访问头以支持在浏览器中直接访问。 |
| comments_setting | -                     | Object  | 评论相关设置                                                 |
|                  | enable_pagination     | Boolean | 是否开启浏览器端分页显示评论，只有这个设置为true以下两个设置才可以生效 |
|                  | max_comments_per_page | Integer | 浏览器端一页最多可以显示的评论数,超过这个数量将自动分页显示  |
|                  | max_comment_char_num  | Integer | 每一条评论的最大字符数限制                                   |
| blog_setting     | -                     | Object  | 博主相关信息设置。当评论时使用owner_name和owner_email进行留言时，会显示owner_avatar头像和owner_show_name 区别于其他人自动生成的8位图片头像，以示区分。 |
|                  | owner_name            | String  | 博主的姓名                                                   |
|                  | owner_email           | String  | 博主的电子邮件地址                                           |
|                  | owner_avatar          | String  | 博主评论时显示的头像                                         |
|                  | owner_show_name       | String  | 博主评论时显示的名字                                         |



## 浏览器端使用说明

前端的使用需要拷贝[srcs/browser](./srcs/browser "srcs")目录下的js,font,css到您的网站目录，然后在网页中插入如下一段代码：

（记得设置 comments_url=你的评论服务的url ）

```html
<!-- NF-COMMENTS BEGIN -->
	<div class="nf-comments-wrapper">
		<div class="nf-comments-main"></div>
		<div class="nf-pagination"></div>
        <div class="nf-copyright">Comments Powered by&nbsp;<a href="https://nifengz.com" target="_blank">nifengz</a></div>
    </div>
    <script src="/js/md5.min.js"></script>
    <script src="/js/identicon.js"></script>
    <script src="/js/runtime.js"></script>
    <script src="/js/nf_comments_template.js"></script>
    <script src="/js/nf_pagination.js"></script>
	<!-- you need to set comments_url to your comments server url -->
    <script src="/js/nf_comments.js?comments_url=your_comments_server_url"></script>
    <!-- NF-COMMENTS END -->
```