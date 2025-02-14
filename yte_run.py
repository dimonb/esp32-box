import yaml


def proxy_tag(tag):
    class Proxy:
        def __init__(self, value):
            self.value = value

    def constructor(loader, node):
        return Proxy(node.value)

    def representer(dumper, data):
        # Представляем как YAML скаляр с тегом !secret
        return yaml.nodes.ScalarNode(tag=tag, value=data.value, style='')

    # Регистрируем конструктор и representer
    yaml.add_constructor(tag, constructor)
    yaml.add_representer(Proxy, representer)


proxy_tag('!secret')
proxy_tag('!lambda')


import yte
yte.main()